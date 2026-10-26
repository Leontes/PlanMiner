#include "datasets/relations.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
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

// Un lado de una relación: el operando, su valor en cada fila y si cambia en la acción.
struct Side {
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
  return std::string{"("} + comparison + " " + plantraces::to_string(relation.left) + " " +
         to_string(relation.right) + ")";
}

RelationOptions RelationOptions::original() {
  RelationOptions options;
  options.effect_expressions = false;
  options.changing_pairs = false;
  options.known_constants = false;
  return options;
}

std::vector<Relation> numeric_relations(const Table& table,
                                        const std::vector<FluentChange>& changes,
                                        const RelationOptions& options) {
  const std::size_t n = table.transitions;

  const auto side = [&](Operand operand) {
    Side result{std::move(operand), {}, false};
    result.values = values(table, result.operand);
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

  std::vector<Side> fluents;
  for (const Attribute& attribute : table.attributes) {
    if (attribute.kind == AttributeKind::fluent) {
      fluents.push_back(side(fluent_operand(attribute.term)));
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
      Operand operand{OperandKind::expression, effect.expression, effect.variables};
      // La misma expresión en dos efectos (en zenotravel, fuel y total-fuel-used al volar).
      std::string text = to_string(operand);
      if (std::find(seen.begin(), seen.end(), text) != seen.end()) {
        continue;
      }
      seen.push_back(std::move(text));
      others.push_back(side(std::move(operand)));
    }
  } else {
    for (const Side& fluent : fluents) {
      if (fluent.changes) {
        others.push_back(
            side({OperandKind::fall, Expression::variable(0), fluent.operand.variables}));
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
      Relation relation{comparison, left.operand.variables.front(), right.operand,
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

  for (std::size_t f = 0; f < fluents.size(); ++f) {
    const Side& left = fluents[f];
    for (std::size_t g = f + 1; g < fluents.size(); ++g) {
      if (wanted(left, fluents[g])) {
        compare(left, fluents[g]);
      }
    }
    for (const Side& other : others) {
      // El original no compara un fluente con lo que baja él mismo.
      const bool itself = other.operand.kind == OperandKind::fall &&
                          other.operand.variables.front() == left.operand.variables.front();
      if (!itself && wanted(left, other)) {
        compare(left, other);
      }
    }
  }
  return relations;
}

}  // namespace datasets
