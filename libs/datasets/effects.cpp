#include "datasets/effects.hpp"

#include <cmath>
#include <utility>

#include "changes.hpp"

namespace datasets {

namespace {

using regression::Expression;
using regression::Operation;

std::vector<double> rows(const std::vector<double>& column, std::size_t from, std::size_t count) {
  const auto start = column.begin() + static_cast<std::ptrdiff_t>(from);
  return {start, start + static_cast<std::ptrdiff_t>(count)};
}

// Una asignación f + E, E + f o f - E (f, la variable del propio fluente) es un increase o un
// decrease de E.
std::optional<std::pair<EffectKind, Expression>> relative(const Expression& expression,
                                                          std::size_t self) {
  const auto is_self = [self](const Expression& operand) {
    return operand.operation() == Operation::variable && operand.index() == self;
  };
  if (expression.operation() == Operation::add) {
    if (is_self(expression.left())) {
      return std::pair{EffectKind::increase, expression.right()};
    }
    if (is_self(expression.right())) {
      return std::pair{EffectKind::increase, expression.left()};
    }
  }
  if (expression.operation() == Operation::subtract && is_self(expression.left())) {
    return std::pair{EffectKind::decrease, expression.right()};
  }
  return std::nullopt;
}

}  // namespace

std::string to_string(const NumericEffect& effect) {
  std::vector<std::string> names;
  for (const plantraces::LiftedAtom& variable : effect.variables) {
    names.push_back(plantraces::to_string(variable));
  }
  const char* kind = "assign";
  switch (effect.kind) {
    case EffectKind::increase:
      kind = "increase";
      break;
    case EffectKind::decrease:
      kind = "decrease";
      break;
    case EffectKind::assign:
      break;
    case EffectKind::magnitude:
      kind = "magnitude";
      break;
  }
  return std::string{"("} + kind + " " + plantraces::to_string(effect.fluent) + " " +
         effect.expression.to_string(names) + ")";
}

EffectOptions EffectOptions::original() {
  EffectOptions options;
  options.before_state = false;
  options.effect_forms = false;
  options.null_model = false;
  options.search = regression::Options::original();
  return options;
}

std::vector<FluentChange> numeric_effects(const Table& table, const EffectOptions& options) {
  const std::size_t n = table.transitions;
  std::vector<std::size_t> fluents;
  for (std::size_t a = 0; a < table.attributes.size(); ++a) {
    if (table.attributes[a].kind == AttributeKind::fluent) {
      fluents.push_back(a);
    }
  }

  std::vector<FluentChange> result;
  for (const std::size_t target : fluents) {
    const std::vector<double> before = rows(table.columns[target], 0, n);
    const std::vector<double> after = rows(table.columns[target], n, n);

    if (!detail::count_unchanged(table.columns[target], n)
             .changes(options.null_model, options.max_unchanged)) {
      continue;
    }

    // Las variables: los fluentes del estado de antes, o los de después sin el propio.
    regression::Problem problem;
    std::vector<plantraces::LiftedAtom> variables;
    std::optional<std::size_t> self;
    for (const std::size_t fluent : fluents) {
      if (!options.before_state && fluent == target) {
        continue;
      }
      if (fluent == target) {
        self = variables.size();
      }
      problem.variables.push_back(rows(table.columns[fluent], options.before_state ? 0 : n, n));
      variables.push_back(table.attributes[fluent].term);
    }

    if (options.effect_forms) {
      std::vector<double> increase(n);
      std::vector<double> decrease(n);
      for (std::size_t i = 0; i < n; ++i) {
        increase[i] = after[i] - before[i];
        decrease[i] = before[i] - after[i];
      }
      problem.targets = {{increase, std::nullopt}, {decrease, std::nullopt}, {after, self}};
    } else {
      std::vector<double> magnitude(n);
      for (std::size_t i = 0; i < n; ++i) {
        magnitude[i] = std::abs(after[i] - before[i]);
      }
      problem.targets = {{magnitude, std::nullopt}};
    }

    FluentChange change{table.attributes[target].term, std::nullopt,
                        regression::search(problem, options.search)};
    if (const std::optional<Expression>& found = change.search.expression) {
      constexpr EffectKind forms[] = {EffectKind::increase, EffectKind::decrease,
                                      EffectKind::assign};
      NumericEffect effect{
          change.fluent, options.effect_forms ? forms[change.search.target] : EffectKind::magnitude,
          *found, variables, change.search.error};
      if (effect.kind == EffectKind::assign && self) {
        if (auto rewritten = relative(effect.expression, *self)) {
          effect.kind = rewritten->first;
          effect.expression = rewritten->second;
        }
      }
      change.effect = std::move(effect);
    }
    result.push_back(std::move(change));
  }
  return result;
}

}  // namespace datasets
