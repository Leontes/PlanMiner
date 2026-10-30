#include "pddl/simplify.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>

namespace pddl {

namespace {

using plantraces::GroundAtom;
using plantraces::Observation;

std::vector<std::string> split(const std::string& text) {
  std::istringstream in(text);
  std::vector<std::string> tokens;
  for (std::string token; in >> token;) {
    tokens.push_back(token);
  }
  return tokens;
}

// Una expresión de un nombre del original: un número, un átomo o un fluente sobre los parámetros
// ("( distance ?param_1 ~ ?city ?param_2 ~ ?city )") o una operación con dos operandos.
struct Expression {
  enum class Kind { number, term, operation };
  Kind kind = Kind::number;
  double number = 0.0;
  // El nombre del término o la operación (+, -, *, /).
  std::string name;
  std::vector<std::size_t> parameters;
  std::vector<Expression> operands;
};

// La expresión que empieza en tokens[i] (y avanza i); nullopt si no tiene esa forma.
std::optional<Expression> parse(const std::vector<std::string>& tokens, std::size_t& i) {
  if (i >= tokens.size()) {
    return std::nullopt;
  }
  if (tokens[i] != "(") {
    try {
      std::size_t used = 0;
      const double number = std::stod(tokens[i], &used);
      if (used != tokens[i].size()) {
        return std::nullopt;
      }
      ++i;
      return Expression{Expression::Kind::number, number, {}, {}, {}};
    } catch (const std::exception&) {
      return std::nullopt;
    }
  }
  if (++i >= tokens.size()) {
    return std::nullopt;
  }
  Expression expression;
  expression.name = tokens[i++];
  if (expression.name == "+" || expression.name == "-" || expression.name == "*" ||
      expression.name == "/") {
    expression.kind = Expression::Kind::operation;
    for (int k = 0; k < 2; ++k) {
      std::optional<Expression> operand = parse(tokens, i);
      if (!operand) {
        return std::nullopt;
      }
      expression.operands.push_back(std::move(*operand));
    }
  } else {
    expression.kind = Expression::Kind::term;
    while (i < tokens.size() && tokens[i] != ")") {
      if (!tokens[i].starts_with("?param_")) {
        return std::nullopt;
      }
      expression.parameters.push_back(std::stoul(tokens[i].substr(7)));
      // El tipo: "~ ?tipo".
      i += (i + 1 < tokens.size() && tokens[i + 1] == "~") ? std::size_t{3} : std::size_t{1};
    }
  }
  if (i >= tokens.size() || tokens[i] != ")") {
    return std::nullopt;
  }
  ++i;
  return expression;
}

void parameters_of(const Expression& expression, std::set<std::size_t>& found,
                   std::set<std::string>& symbols) {
  found.insert(expression.parameters.begin(), expression.parameters.end());
  if (expression.kind == Expression::Kind::term) {
    symbols.insert(expression.name);
  }
  for (const Expression& operand : expression.operands) {
    parameters_of(operand, found, symbols);
  }
}

// Una condición que se escribe en el dominio: un átomo cierto o una comparación cierta.
struct Condition {
  bool comparison = false;
  // opEQ, opLT, opGT, opLET u opGET.
  std::string op;
  Expression left;
  Expression right;
  std::set<std::size_t> parameters;
  // Los predicados y fluentes que usa.
  std::set<std::string> symbols;
};

std::optional<Condition> condition(const rules::Literal& literal) {
  const std::vector<std::string> tokens = split(literal.attribute);
  if (tokens.empty() || literal.value != "True") {
    return std::nullopt;
  }
  Condition result;
  std::size_t i = 0;
  if (tokens[0] == "(") {
    std::optional<Expression> atom = parse(tokens, i);
    if (!atom || atom->kind != Expression::Kind::term || i != tokens.size()) {
      return std::nullopt;
    }
    result.left = std::move(*atom);
  } else {
    static const std::set<std::string> comparisons{"opEQ", "opLT", "opGT", "opLET", "opGET"};
    if (!comparisons.contains(tokens[0]) || tokens.size() < 2 || tokens[1] != "-") {
      return std::nullopt;
    }
    i = 2;
    std::optional<Expression> left = parse(tokens, i);
    std::optional<Expression> right = parse(tokens, i);
    if (!left || !right || i != tokens.size()) {
      return std::nullopt;
    }
    result.comparison = true;
    result.op = tokens[0];
    result.left = std::move(*left);
    result.right = std::move(*right);
  }
  parameters_of(result.left, result.parameters, result.symbols);
  parameters_of(result.right, result.parameters, result.symbols);
  return result;
}

// El objeto de cada parámetro.
using Binding = std::vector<std::string>;

GroundAtom ground(const Expression& term, const Binding& binding) {
  GroundAtom atom{term.name, {}};
  for (const std::size_t parameter : term.parameters) {
    atom.arguments.push_back(binding[parameter]);
  }
  return atom;
}

std::optional<double> value(const Expression& expression, const Observation& state,
                            const Binding& binding) {
  switch (expression.kind) {
    case Expression::Kind::number:
      return expression.number;
    case Expression::Kind::term:
      return state.value(ground(expression, binding));
    case Expression::Kind::operation:
      break;
  }
  const std::optional<double> a = value(expression.operands[0], state, binding);
  const std::optional<double> b = value(expression.operands[1], state, binding);
  if (!a || !b) {
    return std::nullopt;
  }
  switch (expression.name[0]) {
    case '+':
      return *a + *b;
    case '-':
      return *a - *b;
    case '*':
      return *a * *b;
    default:
      return *b == 0.0 ? std::nullopt : std::optional<double>{*a / *b};
  }
}

std::optional<bool> holds(const Condition& condition, const Observation& state,
                          const Binding& binding) {
  if (!condition.comparison) {
    return state.truth(ground(condition.left, binding));
  }
  const std::optional<double> a = value(condition.left, state, binding);
  const std::optional<double> b = value(condition.right, state, binding);
  if (!a || !b) {
    return std::nullopt;
  }
  if (condition.op == "opEQ") {
    return *a == *b;
  }
  if (condition.op == "opLT") {
    return *a < *b;
  }
  if (condition.op == "opGT") {
    return *a > *b;
  }
  if (condition.op == "opLET") {
    return *a <= *b;
  }
  return *a >= *b;
}

// La búsqueda de un contraejemplo en un estado: objetos con los que se cumplen las premisas y no
// la condición. Se asignan los parámetros en orden, y cada condición se comprueba en cuanto tiene
// todos los suyos.
class Search {
 public:
  Search(const std::vector<const Condition*>& premises, const Condition& target,
         const std::vector<std::vector<std::string>>& domains)
      : domains_(domains) {
    std::set<std::size_t> used = target.parameters;
    for (const Condition* premise : premises) {
      used.insert(premise->parameters.begin(), premise->parameters.end());
    }
    order_.assign(used.begin(), used.end());
    checks_.resize(order_.size() + 1);
    const auto add = [this](const Condition* condition, bool is_target) {
      std::size_t depth = 0;
      for (std::size_t d = 0; d < order_.size(); ++d) {
        if (condition->parameters.contains(order_[d])) {
          depth = d + 1;
        }
      }
      checks_[depth].push_back({condition, is_target});
    };
    for (const Condition* premise : premises) {
      add(premise, false);
    }
    add(&target, true);
  }

  bool found(const Observation& state, std::size_t parameters) {
    state_ = &state;
    binding_.assign(parameters, "");
    return passes(0) && search(0);
  }

 private:
  // Las condiciones que se comprueban al asignar el parámetro de la profundidad (0, las que no
  // tienen parámetros).
  bool passes(std::size_t depth) const {
    for (const auto& [condition, is_target] : checks_[depth]) {
      const std::optional<bool> truth = holds(*condition, *state_, binding_);
      if (!truth || *truth == is_target) {
        return false;
      }
    }
    return true;
  }

  bool search(std::size_t depth) {
    if (depth == order_.size()) {
      return true;
    }
    for (const std::string& object : domains_[order_[depth]]) {
      binding_[order_[depth]] = object;
      if (passes(depth + 1) && search(depth + 1)) {
        return true;
      }
    }
    return false;
  }

  const std::vector<std::vector<std::string>>& domains_;
  std::vector<std::size_t> order_;
  std::vector<std::vector<std::pair<const Condition*, bool>>> checks_;
  const Observation* state_ = nullptr;
  Binding binding_;
};

}  // namespace

std::vector<rules::Literal> drop_implied(std::vector<rules::Literal> precondition,
                                         const std::vector<plantraces::Transition>& transitions,
                                         const std::vector<plantraces::Trace>& traces) {
  if (transitions.empty()) {
    return precondition;
  }
  // Los tipos con que aparece cada parámetro en las aplicaciones.
  const std::size_t arity = transitions.front().action->arguments.size();
  std::vector<std::set<std::string>> types(arity);
  for (const plantraces::Transition& transition : transitions) {
    for (std::size_t p = 0; p < arity && p < transition.action->arguments.size(); ++p) {
      types[p].insert(transition.trace->objects.at(transition.action->arguments[p]).front());
    }
  }
  // Los objetos de cada parámetro en cada traza.
  std::vector<std::vector<std::vector<std::string>>> domains;
  for (const plantraces::Trace& trace : traces) {
    std::vector<std::vector<std::string>>& here = domains.emplace_back(arity);
    for (const auto& [object, declared] : trace.objects) {
      for (std::size_t p = 0; p < arity; ++p) {
        if (types[p].contains(declared.front())) {
          here[p].push_back(object);
        }
      }
    }
  }
  // Los predicados y fluentes que cambian en cada traza: si una comprobación no usa ninguno, basta
  // con un estado. Con huecos no se sabe qué cambia, y cuenta todo ("").
  std::vector<std::set<std::string>> changing(traces.size());
  for (std::size_t t = 0; t < traces.size(); ++t) {
    const std::vector<Observation>& states = traces[t].states;
    for (std::size_t i = 0; i + 1 < states.size(); ++i) {
      const Observation& before = states[i];
      const Observation& after = states[i + 1];
      if (before.missing || after.missing || !before.unknown_atoms.empty() ||
          !after.unknown_atoms.empty() || !before.unknown_values.empty() ||
          !after.unknown_values.empty()) {
        changing[t].insert("");
        continue;
      }
      std::vector<GroundAtom> difference;
      std::set_symmetric_difference(before.true_atoms.begin(), before.true_atoms.end(),
                                    after.true_atoms.begin(), after.true_atoms.end(),
                                    std::back_inserter(difference));
      for (const GroundAtom& atom : difference) {
        changing[t].insert(atom.name);
      }
      for (const auto& [term, value] : after.values) {
        if (before.value(term) != value) {
          changing[t].insert(term.name);
        }
      }
    }
  }
  // Si las premisas implican la condición en todos los estados de las trazas.
  const auto implied = [&](const std::vector<const Condition*>& premises, const Condition& target) {
    std::set<std::string> symbols = target.symbols;
    for (const Condition* condition : premises) {
      symbols.insert(condition->symbols.begin(), condition->symbols.end());
    }
    for (std::size_t t = 0; t < traces.size(); ++t) {
      const bool dynamic =
          changing[t].contains("") ||
          std::any_of(symbols.begin(), symbols.end(),
                      [&](const std::string& symbol) { return changing[t].contains(symbol); });
      Search search(premises, target, domains[t]);
      for (const Observation& state : traces[t].states) {
        if (state.missing) {
          continue;
        }
        if (search.found(state, arity)) {
          return false;
        }
        if (!dynamic) {
          break;
        }
      }
    }
    return true;
  };

  std::vector<std::optional<Condition>> conditions;
  for (const rules::Literal& literal : precondition) {
    conditions.push_back(condition(literal));
  }
  // Se prueban de la última a la primera en el orden canónico: por los parámetros y por el nombre.
  std::vector<std::size_t> order;
  for (std::size_t k = 0; k < conditions.size(); ++k) {
    if (conditions[k]) {
      order.push_back(k);
    }
  }
  const auto key = [&](std::size_t k) {
    std::vector<std::size_t> parameters;
    const std::function<void(const Expression&)> collect = [&](const Expression& expression) {
      parameters.insert(parameters.end(), expression.parameters.begin(),
                        expression.parameters.end());
      for (const Expression& operand : expression.operands) {
        collect(operand);
      }
    };
    collect(conditions[k]->left);
    collect(conditions[k]->right);
    return std::make_tuple(conditions[k]->comparison, parameters, precondition[k].attribute);
  };
  std::sort(order.begin(), order.end(),
            [&](std::size_t a, std::size_t b) { return key(a) < key(b); });

  std::vector<bool> kept(precondition.size(), true);
  // Las demás condiciones que se quedan.
  const auto others = [&](std::size_t skip) {
    std::vector<const Condition*> premises;
    for (const std::size_t j : order) {
      if (j != skip && kept[j]) {
        premises.push_back(&*conditions[j]);
      }
    }
    return premises;
  };
  for (auto it = order.rbegin(); it != order.rend(); ++it) {
    const std::size_t k = *it;
    if (conditions[k]->comparison) {
      kept[k] = !implied(others(k), *conditions[k]);
      continue;
    }
    // Un átomo, si es equivalente a otro que se queda del mismo predicado y con los mismos
    // parámetros. Por sí solos: dadas las demás condiciones, dos átomos que estas implican, como
    // (ball ?0) y (gripper ?2), serían equivalentes. Con los mismos parámetros: dos que se cumplen
    // siempre, como (available-color ?1) y (available-color ?2), también lo serían. Y del mismo
    // predicado: de (up ?1 ?2) y (down ?2 ?1) los datos no dicen cuál escribe el dominio.
    for (const std::size_t m : order) {
      if (m == k || !kept[m] || conditions[m]->comparison ||
          conditions[m]->left.name != conditions[k]->left.name ||
          conditions[m]->parameters != conditions[k]->parameters) {
        continue;
      }
      if (implied({&*conditions[m]}, *conditions[k]) &&
          implied({&*conditions[k]}, *conditions[m])) {
        kept[k] = false;
        break;
      }
    }
  }

  std::vector<rules::Literal> result;
  for (std::size_t k = 0; k < precondition.size(); ++k) {
    if (kept[k]) {
      result.push_back(std::move(precondition[k]));
    }
  }
  return result;
}

}  // namespace pddl
