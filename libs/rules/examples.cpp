#include "rules/examples.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace rules {

namespace {

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();

inslv::FuzzySet crisp(const char* label, double value) {
  return {label, value, value, value, value};
}

const std::vector<inslv::FuzzySet>& logical() {
  static const std::vector<inslv::FuzzySet> sets{crisp("False", 0.0), crisp("True", 1.0)};
  return sets;
}

std::string name(const datasets::Attribute& term) {
  return term.kind == datasets::AttributeKind::time ? "time" : plantraces::to_string(term.term);
}

void add(Examples& examples, Attribute attribute, std::vector<inslv::FuzzySet> labels,
         std::vector<double> column) {
  examples.data.attributes.push_back({to_string(attribute), std::move(labels)});
  examples.data.values.push_back(std::move(column));
  examples.attributes.push_back(std::move(attribute));
}

// La expresión del cambio del instante, con los fluentes del estado de después como variables (en
// el original, todos los atributos numéricos menos el instante). Como el original, sobre el valor
// absoluto del cambio.
std::optional<std::pair<regression::Expression, std::vector<plantraces::LiftedAtom>>> time_change(
    const datasets::Table& table, const std::vector<double>& time,
    const regression::Options& search) {
  const std::size_t n = table.transitions;
  regression::Problem problem;
  std::vector<plantraces::LiftedAtom> variables;
  for (std::size_t a = 0; a < table.attributes.size(); ++a) {
    if (table.attributes[a].kind == datasets::AttributeKind::fluent) {
      const std::vector<double>& column = table.columns[a];
      problem.variables.emplace_back(column.begin() + static_cast<std::ptrdiff_t>(n), column.end());
      variables.push_back(table.attributes[a].term);
    }
  }
  std::vector<double> magnitude(n);
  for (std::size_t i = 0; i < n; ++i) {
    magnitude[i] = std::abs(time[n + i] - time[i]);
  }
  problem.targets = {{magnitude, std::nullopt}};
  const regression::Result result = regression::search(problem, search);
  if (!result.expression) {
    return std::nullopt;
  }
  return std::pair{*result.expression, variables};
}

}  // namespace

std::string to_string(const Attribute& attribute) {
  switch (attribute.kind) {
    case AttributeKind::atom:
      return plantraces::to_string(attribute.term.term);
    case AttributeKind::value:
      return "value " + name(attribute.term);
    case AttributeKind::relation:
      return datasets::to_string(*attribute.relation);
    case AttributeKind::change: {
      std::vector<std::string> names;
      for (const plantraces::LiftedAtom& variable : attribute.variables) {
        names.push_back(plantraces::to_string(variable));
      }
      return "change " + name(attribute.term) + " " + attribute.expression->to_string(names);
    }
  }
  return "";
}

ExampleOptions ExampleOptions::original() {
  ExampleOptions options;
  options.changes = true;
  options.time = true;
  options.time_search = regression::Options::original();
  return options;
}

namespace {

Examples empty_examples(const datasets::Table& table) {
  Examples examples;
  examples.data.target = {"Class", {crisp("PreState", 0.0), crisp("PostState", 1.0)}};
  examples.data.classes.assign(table.rows(), 0);
  std::fill(examples.data.classes.begin() + static_cast<std::ptrdiff_t>(table.transitions),
            examples.data.classes.end(), 1);
  return examples;
}

}  // namespace

Examples logic_examples(const datasets::Table& table) {
  Examples result = empty_examples(table);
  for (std::size_t a = 0; a < table.attributes.size(); ++a) {
    const datasets::Attribute& attribute = table.attributes[a];
    if (attribute.kind == datasets::AttributeKind::atom) {
      add(result, {AttributeKind::atom, attribute, std::nullopt, std::nullopt, {}}, logical(),
          table.columns[a]);
    }
  }
  return result;
}

Examples numeric_examples(const datasets::Table& table,
                          const std::vector<datasets::FluentChange>& changes,
                          const std::vector<datasets::Relation>& relations,
                          const datasets::Random& random, const ExampleOptions& options) {
  const std::size_t n = table.transitions;
  Examples result = empty_examples(table);

  // Los atributos numéricos: los fluentes y, si cuenta, el instante.
  std::vector<std::size_t> numeric;
  for (std::size_t a = 0; a < table.attributes.size(); ++a) {
    const datasets::AttributeKind kind = table.attributes[a].kind;
    if (kind == datasets::AttributeKind::fluent ||
        (kind == datasets::AttributeKind::time && options.time)) {
      numeric.push_back(a);
    }
  }

  for (const std::size_t a : numeric) {
    std::vector<inslv::FuzzySet> labels;
    for (const datasets::FuzzySet& set : datasets::discretize(table.columns[a], random)) {
      labels.push_back({"L_" + std::to_string(labels.size()), set.a, set.b, set.c, set.d});
    }
    add(result, {AttributeKind::value, table.attributes[a], std::nullopt, std::nullopt, {}},
        std::move(labels), table.columns[a]);
  }

  for (const datasets::Relation& relation : relations) {
    datasets::Relation description = relation;
    description.column.clear();
    add(result, {AttributeKind::relation, relation.left, std::move(description), std::nullopt, {}},
        logical(), relation.column);
  }

  if (!options.changes) {
    return result;
  }
  for (const std::size_t a : numeric) {
    const datasets::Attribute& term = table.attributes[a];
    const std::vector<double>& values = table.columns[a];
    std::vector<double> column(table.rows(), 0.0);
    bool changes_value = false;
    for (std::size_t i = 0; i < n; ++i) {
      const double before = values[i];
      const double after = values[n + i];
      if (std::isnan(before) || std::isnan(after)) {
        column[n + i] = unknown;
      } else {
        column[n + i] = after - before > 0.0 ? 1.0 : -1.0;
        changes_value = changes_value || after != before;
      }
    }
    if (!changes_value) {
      continue;
    }

    Attribute attribute{AttributeKind::change, term, std::nullopt, std::nullopt, {}};
    if (term.kind == datasets::AttributeKind::time) {
      auto found = time_change(table, values, options.time_search);
      if (!found) {
        continue;
      }
      attribute.expression = std::move(found->first);
      attribute.variables = std::move(found->second);
    } else {
      const auto change = std::find_if(
          changes.begin(), changes.end(),
          [&](const datasets::FluentChange& c) { return c.fluent == term.term && c.effect; });
      if (change == changes.end()) {
        continue;
      }
      attribute.expression = change->effect->expression;
      attribute.variables = change->effect->variables;
    }

    std::vector<inslv::FuzzySet> labels{crisp("NA", 0.0)};
    if (std::find(column.begin(), column.end(), 1.0) != column.end()) {
      labels.push_back(crisp("POS", 1.0));
    }
    if (std::find(column.begin(), column.end(), -1.0) != column.end()) {
      labels.push_back(crisp("NEG", -1.0));
    }
    add(result, std::move(attribute), std::move(labels), std::move(column));
  }
  return result;
}

std::vector<Rule> learn(const Examples& examples, const inslv::Options& options) {
  if (examples.data.attributes.empty()) {
    return {};
  }
  std::vector<Rule> rules;
  for (const inslv::Rule& learned : inslv::learn(examples.data, options)) {
    Rule rule;
    rule.post = examples.data.target.labels[learned.consequent].label == "PostState";
    rule.covered = static_cast<int>(learned.covered);
    for (const inslv::Condition& condition : learned.antecedent) {
      const std::vector<inslv::FuzzySet>& labels =
          examples.data.attributes[condition.attribute].labels;
      if (labels.size() > 2) {
        continue;
      }
      for (const std::size_t label : condition.labels) {
        const inslv::FuzzySet& set = labels[label];
        if (set.label == "True" || set.label == "False") {
          rule.conditions.push_back({condition.attribute, set.label == "True"});
        } else {
          rule.conditions.push_back({condition.attribute, static_cast<int>((set.b + set.c) / 2.0)});
        }
      }
    }
    rules.push_back(std::move(rule));
  }
  return rules;
}

}  // namespace rules
