// Oráculo del aprendizaje de reglas: los ejemplos de INSLV (rules::logic_examples y
// rules::numeric_examples) y las reglas interpretadas (rules::learn), con la configuración
// original, frente al pipeline del PlanMiner original hasta INSLV: to_table, discretize, inslv()
// y su interpretación (SplitAntecedent), como en Old/src/bin/mainPlanMiner.cpp.
//
// Los dos lados usan el rand() global (en la discretización y en INSLV), así que cada uno recorre
// todas las acciones en el orden del original empezando con la misma semilla, y después se
// comparan.
//
// Solo con trazas en las que el A* del original es inmediato: blocksworld (sin fluentes) y
// counters (cambios de 1) sin ruido. Con zenotravel el original tarda cientos de segundos y
// gigas de memoria, y con ruido lanza el A* sobre los cambios que mete el ruido.

#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <list>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "datasets/effects.hpp"
#include "datasets/relations.hpp"
#include "datasets/table.hpp"
#include "legacy.hpp"
#include "legacy_rules.hpp"
#include "plantraces/actions.hpp"
#include "pts.hpp"
#include "rules/examples.hpp"

namespace {

namespace fs = std::filesystem;

using oracle::Case;
using oracle::legacy_label;
using oracle::legacy_missing;
using oracle::legacy_relation_label;
using oracle::LegacyExamples;

// La expresión del cambio como la escribe el original. Solo hace falta con constantes: en estas
// trazas los cambios son de 1.
std::string legacy_formula(const regression::Expression& expression,
                           const std::vector<plantraces::LiftedAtom>& variables) {
  if (expression.operation() == regression::Operation::constant &&
      expression.value() == std::floor(expression.value())) {
    return std::to_string(static_cast<long long>(expression.value())) + " ";
  }
  std::vector<std::string> names;
  for (const plantraces::LiftedAtom& variable : variables) {
    names.push_back(plantraces::to_string(variable));
  }
  return expression.to_string(names) + " ";
}

// El nombre que da el original a un atributo. En las relaciones con lo que baja un fluente, el
// original pone la expresión de su cambio (o el fluente, si no la tiene).
std::string legacy_name(const rules::Attribute& attribute, const rules::Examples& examples,
                        const plantraces::Transition& transition) {
  switch (attribute.kind) {
    case rules::AttributeKind::atom:
    case rules::AttributeKind::value:
      return legacy_label(attribute.term, transition);
    case rules::AttributeKind::relation: {
      const datasets::Relation& relation = *attribute.relation;
      if (relation.right.kind != datasets::OperandKind::fall) {
        return legacy_relation_label(relation, transition);
      }
      const datasets::Attribute fluent{datasets::AttributeKind::fluent,
                                       relation.right.variables.front()};
      std::string right = legacy_label(fluent, transition);
      for (const rules::Attribute& change : examples.attributes) {
        if (change.kind == rules::AttributeKind::change && change.term == fluent) {
          right = legacy_formula(*change.expression, change.variables);
        }
      }
      const std::string label = legacy_relation_label(relation, transition);
      return label.substr(0, label.find("  ") + 2) + right;
    }
    case rules::AttributeKind::change:
      return "DELTA - " + legacy_label(attribute.term, transition) + " " +
             legacy_formula(*attribute.expression, attribute.variables);
  }
  return "";
}

// Los ejemplos y las reglas del lado nuevo, con los nombres del original.
LegacyExamples as_legacy(const rules::Examples& examples, const std::vector<rules::Rule>& rules,
                         const plantraces::Transition& transition) {
  LegacyExamples result;
  for (std::size_t a = 0; a < examples.attributes.size(); ++a) {
    result.names.push_back(legacy_name(examples.attributes[a], examples, transition));
    std::vector<oracle::LegacyLabel> labels;
    for (const inslv::FuzzySet& set : examples.data.attributes[a].labels) {
      labels.push_back({set.label, set.a, set.b, set.c, set.d});
    }
    result.labels.push_back(labels);
    std::vector<double> values = examples.data.values[a];
    for (double& value : values) {
      if (std::isnan(value)) {
        value = legacy_missing;
      }
    }
    result.values.push_back(values);
  }
  for (const std::size_t c : examples.data.classes) {
    result.classes.push_back(static_cast<double>(c));
  }
  for (const rules::Rule& rule : rules) {
    std::list<std::pair<std::string, std::string>> conditions;
    for (const rules::Condition& condition : rule.conditions) {
      const std::string value = std::holds_alternative<bool>(condition.value)
                                    ? (std::get<bool>(condition.value) ? "True" : "False")
                                    : std::to_string(std::get<int>(condition.value));
      conditions.emplace_back(result.names[condition.attribute], value);
    }
    result.rules.emplace_back(rule.post ? rule.covered : -rule.covered, conditions);
  }
  return result;
}

void expect_same(const LegacyExamples& actual, const std::optional<LegacyExamples>& legacy) {
  if (!legacy) {
    EXPECT_TRUE(actual.names.empty());
    EXPECT_TRUE(actual.rules.empty());
    return;
  }
  const LegacyExamples& expected = *legacy;
  ASSERT_EQ(actual.names, expected.names);
  for (std::size_t a = 0; a < actual.names.size(); ++a) {
    SCOPED_TRACE(actual.names[a]);
    ASSERT_EQ(actual.labels[a].size(), expected.labels[a].size());
    for (std::size_t l = 0; l < actual.labels[a].size(); ++l) {
      const oracle::LegacyLabel& set = actual.labels[a][l];
      const oracle::LegacyLabel& original = expected.labels[a][l];
      EXPECT_EQ(set.label, original.label);
      EXPECT_EQ(set.a, original.a) << set.label;
      EXPECT_EQ(set.b, original.b) << set.label;
      EXPECT_EQ(set.c, original.c) << set.label;
      EXPECT_EQ(set.d, original.d) << set.label;
    }
    EXPECT_EQ(actual.values[a], expected.values[a]);
  }
  EXPECT_EQ(actual.classes, expected.classes);
  ASSERT_EQ(actual.rules.size(), expected.rules.size());
  for (std::size_t r = 0; r < actual.rules.size(); ++r) {
    EXPECT_EQ(actual.rules[r], expected.rules[r]) << "regla " << r;
  }
}

class LegacyRules : public testing::TestWithParam<Case> {};

TEST_P(LegacyRules, SameExamplesAndRulesAsTheOriginal) {
  const std::vector<plantraces::Trace> traces = oracle::read_traces(GetParam());
  ASSERT_FALSE(traces.empty());
  const fs::path work =
      fs::path{PLANMINER_ORACLE_WORK_DIR} /
      (std::string{testing::UnitTest::GetInstance()->current_test_info()->name()} + ".pts");
  fs::create_directories(work.parent_path());
  std::ofstream(work) << oracle::to_pts(traces);
  fs::create_directories("res");

  std::srand(1);
  auto expected = oracle::legacy_rules(work);

  // El nuevo, en el mismo orden: por acción, el conjunto lógico y después el numérico.
  const plantraces::Vocabulary vocabulary = plantraces::vocabulary(traces);
  const auto actions = plantraces::group_by_action(traces);
  ASSERT_EQ(actions.size(), expected.size());
  std::srand(1);
  for (const auto& [name, transitions] : actions) {
    SCOPED_TRACE("acción " + name);
    ASSERT_TRUE(expected.contains(name));
    const datasets::Table table = datasets::build_table(vocabulary, transitions);

    const rules::Examples logic = rules::logic_examples(table);
    const std::vector<rules::Rule> logic_rules = rules::learn(logic);
    {
      SCOPED_TRACE("lógico");
      expect_same(as_legacy(logic, logic_rules, transitions.front()), expected[name][0]);
    }

    const std::vector<datasets::FluentChange> changes =
        datasets::numeric_effects(table, datasets::EffectOptions::original());
    const std::vector<datasets::Relation> relations =
        datasets::numeric_relations(table, {}, datasets::RelationOptions::original());
    const rules::Examples numeric = rules::numeric_examples(
        table, changes, relations, [] { return std::rand(); }, rules::ExampleOptions::original());
    const std::vector<rules::Rule> numeric_rules = rules::learn(numeric);
    {
      SCOPED_TRACE("numérico");
      expect_same(as_legacy(numeric, numeric_rules, transitions.front()), expected[name][1]);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(Traces, LegacyRules,
                         testing::Values(Case{"blocks-strips-untyped", "complete"},
                                         Case{"blocks-strips-untyped", "missing30"},
                                         Case{"blocks-strips-untyped", "noise5"},
                                         Case{"counters", "complete"},
                                         Case{"counters", "missing30"}),
                         [](const testing::TestParamInfo<Case>& c) {
                           return oracle::case_name(c.param);
                         });

}  // namespace
