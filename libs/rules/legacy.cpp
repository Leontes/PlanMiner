#include "rules/legacy.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <variant>

namespace rules {

std::string legacy_label(const datasets::Attribute& term, const std::vector<std::string>& types) {
  if (term.kind == datasets::AttributeKind::time) {
    return "TIMESTAMP";
  }
  std::string label = "( " + term.term.name + " ";
  for (const std::size_t parameter : term.term.parameters) {
    label += "?param_" + std::to_string(parameter) + " ~ " + types.at(parameter) + " ";
  }
  return label + ")";
}

std::string legacy_formula(const regression::Expression& expression,
                           const std::vector<plantraces::LiftedAtom>& variables,
                           const std::vector<std::string>& types) {
  switch (expression.operation()) {
    case regression::Operation::constant: {
      // Las constantes del original son enteros.
      const double value = expression.value();
      if (value == std::floor(value)) {
        return std::to_string(static_cast<long long>(value));
      }
      std::ostringstream out;
      out << value;
      return out.str();
    }
    case regression::Operation::variable:
      return legacy_label({datasets::AttributeKind::fluent, variables.at(expression.index())},
                          types);
    case regression::Operation::add:
    case regression::Operation::subtract:
    case regression::Operation::multiply:
    case regression::Operation::divide:
      break;
  }
  const char* operation = "+";
  switch (expression.operation()) {
    case regression::Operation::subtract:
      operation = "-";
      break;
    case regression::Operation::multiply:
      operation = "*";
      break;
    case regression::Operation::divide:
      operation = "/";
      break;
    default:
      break;
  }
  return std::string{"( "} + operation + " " + legacy_formula(expression.left(), variables, types) +
         " " + legacy_formula(expression.right(), variables, types) + " )";
}

namespace {

// La expresión del cambio de un fluente o del instante, como la pone el original en los nombres:
// con un espacio al final.
std::string change_formula(const Attribute& change, const std::vector<std::string>& types) {
  return legacy_formula(*change.expression, change.variables, types) + " ";
}

}  // namespace

std::string legacy_name(const Attribute& attribute, const Examples& examples,
                        const std::vector<std::string>& types) {
  switch (attribute.kind) {
    case AttributeKind::atom:
    case AttributeKind::value:
      return legacy_label(attribute.term, types);
    case AttributeKind::change:
      return "DELTA - " + legacy_label(attribute.term, types) + " " +
             change_formula(attribute, types);
    case AttributeKind::relation:
      break;
  }
  const datasets::Relation& relation = *attribute.relation;
  const char* op = "opEQ";
  if (relation.comparison == datasets::Comparison::less) {
    op = "opLT";
  } else if (relation.comparison == datasets::Comparison::greater) {
    op = "opGT";
  }
  std::string right;
  if (relation.right.kind == datasets::OperandKind::fall) {
    const datasets::Attribute fluent{datasets::AttributeKind::fluent,
                                     relation.right.variables.front()};
    right = legacy_label(fluent, types);
    for (const Attribute& change : examples.attributes) {
      if (change.kind == AttributeKind::change && change.term == fluent) {
        right = change_formula(change, types);
      }
    }
  } else if (relation.right.expression.operation() == regression::Operation::variable) {
    right = legacy_label({datasets::AttributeKind::fluent,
                          relation.right.variables.at(relation.right.expression.index())},
                         types);
  } else {
    // La expresión de un efecto (o una constante), como el original escribe la del cambio.
    right = legacy_formula(relation.right.expression, relation.right.variables, types) + " ";
  }
  return std::string{op} + " - " + legacy_label(relation.left, types) + "  " + right;
}

Evidence evidence(const std::vector<const Examples*>& examples,
                  const std::vector<std::string>& types) {
  Evidence result;
  for (const Examples* set : examples) {
    result.transitions = static_cast<std::size_t>(
        std::count(set->data.classes.begin(), set->data.classes.end(), std::size_t{0}));
    for (std::size_t a = 0; a < set->attributes.size(); ++a) {
      result.columns.emplace(legacy_name(set->attributes[a], *set, types), set->data.values[a]);
    }
  }
  return result;
}

std::vector<LegacyRule> legacy_rules(const std::vector<Rule>& rules, const Examples& examples,
                                     const std::vector<std::string>& types) {
  std::vector<std::string> names;
  for (const Attribute& attribute : examples.attributes) {
    names.push_back(legacy_name(attribute, examples, types));
  }
  std::vector<LegacyRule> result;
  for (const Rule& rule : rules) {
    LegacyRule legacy{rule.post ? rule.covered : -rule.covered, {}};
    for (const Condition& condition : rule.conditions) {
      const std::string value = std::holds_alternative<bool>(condition.value)
                                    ? (std::get<bool>(condition.value) ? "True" : "False")
                                    : std::to_string(std::get<int>(condition.value));
      legacy.conditions.push_back({names[condition.attribute], value});
    }
    result.push_back(std::move(legacy));
  }
  return result;
}

}  // namespace rules
