#include "datasets/relations.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <optional>
#include <utility>

#include "changes.hpp"

namespace datasets {

namespace {

using regression::Expression;
using regression::Operation;

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();
constexpr Comparison comparisons[] = {Comparison::equal, Comparison::less, Comparison::greater};

bool holds(Comparison comparison, double left, double right) {
  switch (comparison) {
    case Comparison::equal:
      return left == right;
    case Comparison::less:
      return left < right;
    case Comparison::greater:
      return left > right;
  }
  return false;
}

// (total-cost), el coste del plan con :action-costs.
bool is_total_cost(const plantraces::LiftedAtom& fluent) {
  return fluent.name == "total-cost" && fluent.parameters.empty();
}

// Si la expresión usa alguna variable cuyo fluente cumple predicate.
template <typename Predicate>
bool uses(const Expression& expression, const std::vector<plantraces::LiftedAtom>& variables,
          const Predicate& predicate) {
  switch (expression.operation()) {
    case Operation::constant:
      return false;
    case Operation::variable:
      return predicate(variables[expression.index()]);
    default:
      return uses(expression.left(), variables, predicate) ||
             uses(expression.right(), variables, predicate);
  }
}

// Si la relación vale distinto en alguna de las filas en que se conoce.
bool varies(const std::vector<double>& column) {
  std::optional<double> first;
  for (const double value : column) {
    if (std::isnan(value)) {
      continue;
    }
    if (first && *first != value) {
      return true;
    }
    first = value;
  }
  return false;
}

// vectorInutil del original: compara con la primera fila aunque sea desconocida.
bool varies_from_first(const std::vector<double>& column) {
  for (std::size_t row = 1; row < column.size(); ++row) {
    if (!std::isnan(column[row]) && !(column[row] == column[0])) {
      return true;
    }
  }
  return false;
}

std::optional<std::size_t> column_of(const Table& table, const plantraces::LiftedAtom& fluent) {
  for (std::size_t a = 0; a < table.attributes.size(); ++a) {
    if (table.attributes[a].kind == AttributeKind::fluent && table.attributes[a].term == fluent) {
      return a;
    }
  }
  return std::nullopt;
}

// Un lado de una relación: el atributo (a la izquierda) o el operando (a la derecha), su valor en
// cada fila y si cambia en la acción.
struct Side {
  Attribute attribute;
  Operand operand;
  std::vector<double> values;
  bool changes = false;
};

}  // namespace

Operand fluent_operand(const plantraces::LiftedAtom& fluent) {
  return {OperandKind::expression, Expression::variable(0), {fluent}};
}

std::string to_string(const Operand& operand) {
  if (operand.kind == OperandKind::fall) {
    const std::string fluent = plantraces::to_string(operand.variables.front());
    return "(- " + fluent + " " + fluent + "')";
  }
  std::vector<std::string> names;
  for (const plantraces::LiftedAtom& variable : operand.variables) {
    names.push_back(plantraces::to_string(variable));
  }
  return operand.expression.to_string(names);
}

std::vector<double> values(const Table& table, const Operand& operand) {
  std::vector<std::vector<double>> variables;
  for (const plantraces::LiftedAtom& fluent : operand.variables) {
    const std::optional<std::size_t> column = column_of(table, fluent);
    variables.push_back(column ? table.columns[*column]
                               : std::vector<double>(table.rows(), unknown));
  }
  std::vector<double> result = operand.expression.evaluate(variables, table.rows());
  if (operand.kind == OperandKind::fall) {
    const std::size_t n = table.transitions;
    for (std::size_t i = 0; i < n; ++i) {
      const double fall = result[i] - result[n + i];
      result[i] = fall;
      result[n + i] = fall;
    }
  }
  return result;
}

std::string to_string(const Relation& relation) {
  const char* comparison = "=";
  switch (relation.comparison) {
    case Comparison::equal:
      break;
    case Comparison::less:
      comparison = "<";
      break;
    case Comparison::greater:
      comparison = ">";
      break;
  }
  const std::string left = relation.left.kind == AttributeKind::time
                               ? "time"
                               : plantraces::to_string(relation.left.term);
  return std::string{"("} + comparison + " " + left + " " + to_string(relation.right) + ")";
}

RelationOptions RelationOptions::original() {
  RelationOptions options;
  options.effect_expressions = false;
  options.changing_pairs = false;
  options.known_constants = false;
  options.time = true;
  options.action_costs = false;
  options.accumulators = false;
  return options;
}

std::set<std::string> accumulators(const std::vector<plantraces::Trace>& traces) {
  std::set<std::string> increase;
  std::set<std::string> decrease;
  for (const plantraces::Trace& trace : traces) {
    for (std::size_t i = 0; i + 1 < trace.states.size(); ++i) {
      for (const auto& [term, after] : trace.states[i + 1].values) {
        const std::optional<double> before = trace.states[i].value(term);
        if (before && after > *before) {
          increase.insert(term.name);
        } else if (before && after < *before) {
          decrease.insert(term.name);
        }
      }
    }
  }
  std::set<std::string> result;
  std::set_difference(increase.begin(), increase.end(), decrease.begin(), decrease.end(),
                      std::inserter(result, result.end()));
  return result;
}

std::vector<Relation> numeric_relations(const Table& table,
                                        const std::vector<FluentChange>& changes,
                                        const RelationOptions& options,
                                        const std::set<std::string>& accumulators) {
  const std::size_t n = table.transitions;
  // Lo que no se compara: (total-cost) y los acumuladores, según las opciones.
  const auto excluded = [&](const plantraces::LiftedAtom& fluent) {
    return (options.action_costs && is_total_cost(fluent)) ||
           (options.accumulators && accumulators.contains(fluent.name));
  };

  const auto side = [&](Attribute attribute, Operand operand, std::vector<double> column) {
    Side result{std::move(attribute), std::move(operand), std::move(column), false};
    if (options.changing_pairs) {
      result.changes =
          detail::count_unchanged(result.values, n).changes(true, options.max_unchanged);
    } else {
      // Como el original: si cambia en alguna aplicación, comparando exactamente.
      for (std::size_t i = 0; i < n; ++i) {
        const double before = result.values[i];
        const double after = result.values[n + i];
        result.changes =
            result.changes || (!std::isnan(before) && !std::isnan(after) && before != after);
      }
    }
    return result;
  };

  // Los atributos numéricos: los fluentes y, según las opciones, el instante.
  std::vector<Side> numeric;
  for (std::size_t a = 0; a < table.attributes.size(); ++a) {
    const Attribute& attribute = table.attributes[a];
    if (attribute.kind == AttributeKind::fluent) {
      if (excluded(attribute.term)) {
        continue;
      }
      numeric.push_back(side(attribute, fluent_operand(attribute.term), table.columns[a]));
    } else if (attribute.kind == AttributeKind::time && options.time) {
      numeric.push_back(side(attribute, {OperandKind::expression, Expression::variable(0), {}},
                             table.columns[a]));
    }
  }

  // Con lo que se compara cada fluente, además de con los otros fluentes.
  std::vector<Side> others;
  if (options.effect_expressions) {
    std::vector<std::string> seen;
    for (const FluentChange& change : changes) {
      if (!change.effect) {
        continue;
      }
      const NumericEffect& effect = *change.effect;
      // Un fluente solo ya se compara con los demás fluentes.
      if (effect.expression.operation() == Operation::variable) {
        continue;
      }
      if (uses(effect.expression, effect.variables, excluded)) {
        continue;
      }
      Operand operand{OperandKind::expression, effect.expression, effect.variables};
      // La misma expresión en dos efectos (en zenotravel, fuel y total-fuel-used al volar).
      std::string text = to_string(operand);
      if (std::find(seen.begin(), seen.end(), text) != seen.end()) {
        continue;
      }
      seen.push_back(std::move(text));
      std::vector<double> column = values(table, operand);
      others.push_back(side({}, std::move(operand), std::move(column)));
    }
  } else {
    // Lo que baja el instante no se compara con nada, como en el original.
    for (const Side& fluent : numeric) {
      if (fluent.changes && fluent.attribute.kind == AttributeKind::fluent) {
        Operand operand{OperandKind::fall, Expression::variable(0), {fluent.attribute.term}};
        std::vector<double> column = values(table, operand);
        others.push_back(side({}, std::move(operand), std::move(column)));
      }
    }
  }

  // El original solo compara un fluente que cambia; si no, basta con que cambie un lado.
  const auto wanted = [&options](const Side& left, const Side& right) {
    return left.changes || (options.changing_pairs && right.changes);
  };

  std::vector<Relation> relations;
  const auto compare = [&](const Side& left, const Side& right) {
    for (const Comparison comparison : comparisons) {
      Relation relation{comparison, left.attribute, right.operand,
                        std::vector<double>(table.rows(), unknown)};
      for (std::size_t row = 0; row < table.rows(); ++row) {
        if (!std::isnan(left.values[row]) && !std::isnan(right.values[row])) {
          relation.column[row] = holds(comparison, left.values[row], right.values[row]) ? 1.0 : 0.0;
        }
      }
      if (options.known_constants ? varies(relation.column) : varies_from_first(relation.column)) {
        relations.push_back(std::move(relation));
      }
    }
  };

  // El instante solo va a la izquierda, como en el original.
  for (std::size_t f = 0; f < numeric.size(); ++f) {
    const Side& left = numeric[f];
    for (std::size_t g = f + 1; g < numeric.size(); ++g) {
      if (numeric[g].attribute.kind == AttributeKind::fluent && wanted(left, numeric[g])) {
        compare(left, numeric[g]);
      }
    }
    for (const Side& other : others) {
      // El original no compara un fluente con lo que baja él mismo.
      const bool itself = other.operand.kind == OperandKind::fall &&
                          left.attribute.kind == AttributeKind::fluent &&
                          other.operand.variables.front() == left.attribute.term;
      if (!itself && wanted(left, other)) {
        compare(left, other);
      }
    }
  }
  return relations;
}

}  // namespace datasets
