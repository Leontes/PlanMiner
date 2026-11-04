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
#include <sstream>
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
#include "plantraces/types.hpp"
#include "pts.hpp"
#include "rules/examples.hpp"
#include "rules/fusion.hpp"
#include "rules/legacy.hpp"

namespace {

namespace fs = std::filesystem;

using oracle::Case;
using oracle::legacy_missing;
using oracle::LegacyExamples;

// Los tipos de los argumentos de la primera aplicación de la acción: los de los nombres del
// original.
std::vector<std::string> argument_types(const plantraces::Transition& transition) {
  std::vector<std::string> types;
  for (const std::string& object : transition.action->arguments) {
    types.push_back(transition.trace->objects.at(object).front());
  }
  return types;
}

oracle::RuleList as_rule_list(const std::vector<rules::LegacyRule>& rules) {
  oracle::RuleList result;
  for (const rules::LegacyRule& rule : rules) {
    std::list<std::pair<std::string, std::string>> conditions;
    for (const rules::Literal& literal : rule.conditions) {
      conditions.emplace_back(literal.attribute, literal.value);
    }
    result.emplace_back(rule.covered, conditions);
  }
  return result;
}

// La cabecera de la acción como la da el original: los pares (parámetro, tipo).
std::vector<std::pair<std::string, std::string>> legacy_header(
    const std::vector<std::string>& types) {
  std::vector<std::pair<std::string, std::string>> header;
  for (std::size_t p = 0; p < types.size(); ++p) {
    header.emplace_back("param_" + std::to_string(p), types[p]);
  }
  return header;
}

std::vector<std::pair<std::string, std::string>> as_pairs(
    const std::vector<rules::Literal>& literals) {
  std::vector<std::pair<std::string, std::string>> pairs;
  for (const rules::Literal& literal : literals) {
    pairs.emplace_back(literal.attribute, literal.value);
  }
  return pairs;
}

// Sin las comparaciones del instante con un número ("opGT - TIMESTAMP 1 ..."): el original saca
// su lado izquierdo leyendo fuera de rango hasta encontrar un paréntesis, con lo que quede en
// memoria de otros nombres. Las descarta al escribir el dominio.
std::vector<std::pair<std::string, std::string>> defined(
    std::vector<std::pair<std::string, std::string>> literals) {
  std::erase_if(literals, [](const std::pair<std::string, std::string>& literal) {
    std::istringstream in(literal.first);
    std::string op, dash, left;
    in >> op >> dash >> left;
    return left == "TIMESTAMP";
  });
  return literals;
}

// Que la fusión nueva dé lo mismo que la del original.
void expect_same_fusion(const rules::Fused& fused, const oracle::LegacyFused& legacy) {
  EXPECT_EQ(defined(as_pairs(fused.precondition)), defined(legacy.precondition));
  EXPECT_EQ(defined(as_pairs(fused.effect)), defined(legacy.effect));
}

// Los ejemplos y las reglas del lado nuevo, con los nombres del original.
LegacyExamples as_legacy(const rules::Examples& examples, const std::vector<rules::Rule>& rules,
                         const std::vector<std::string>& types) {
  LegacyExamples result;
  for (std::size_t a = 0; a < examples.attributes.size(); ++a) {
    result.names.push_back(rules::legacy_name(examples.attributes[a], examples, types));
    std::vector<oracle::LegacyLabel> labels;
    for (const inslv::ConjuntoDifuso& set : examples.data.atributos[a].etiquetas) {
      labels.push_back({set.etiqueta, set.a, set.b, set.c, set.d});
    }
    result.labels.push_back(labels);
    std::vector<double> values = examples.data.valores[a];
    for (double& value : values) {
      if (std::isnan(value)) {
        value = legacy_missing;
      }
    }
    result.values.push_back(values);
  }
  for (const std::size_t c : examples.data.clases) {
    result.classes.push_back(static_cast<double>(c));
  }
  result.rules = as_rule_list(rules::legacy_rules(rules, examples, types));
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
  const plantraces::TypeHierarchy hierarchy = plantraces::type_hierarchy(traces);
  const auto actions = plantraces::group_by_action(traces);
  ASSERT_EQ(actions.size(), expected.size());
  std::srand(1);
  for (const auto& [name, transitions] : actions) {
    SCOPED_TRACE("acción " + name);
    ASSERT_TRUE(expected.contains(name));
    const datasets::Table table = datasets::build_table(vocabulary, transitions);
    const std::vector<std::string> types = argument_types(transitions.front());

    const rules::Examples logic = rules::logic_examples(table);
    const std::vector<rules::Rule> logic_rules = rules::learn(logic);
    {
      SCOPED_TRACE("lógico");
      expect_same(as_legacy(logic, logic_rules, types), expected[name][0]);
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
      expect_same(as_legacy(numeric, numeric_rules, types), expected[name][1]);
    }

    // La fusión: la del original con sus reglas y la nueva con las nuevas.
    SCOPED_TRACE("fusión");
    oracle::RuleList legacy_input;
    for (const std::optional<LegacyExamples>& examples : expected[name]) {
      if (examples) {
        legacy_input.insert(legacy_input.end(), examples->rules.begin(), examples->rules.end());
      }
    }
    std::vector<rules::LegacyRule> input = rules::legacy_rules(logic_rules, logic, types);
    const std::vector<rules::LegacyRule> numeric_input =
        rules::legacy_rules(numeric_rules, numeric, types);
    input.insert(input.end(), numeric_input.begin(), numeric_input.end());
    const std::vector<std::string> header =
        hierarchy.actions.contains(name) ? hierarchy.actions.at(name) : std::vector<std::string>{};
    expect_same_fusion(rules::fuse_original(input, header),
                       oracle::legacy_fuse(legacy_input, legacy_header(header)));
  }
}

// La fusión con las reglas que aprende la configuración original nueva: con trazas en las que el
// original tarda demasiado (zenotravel) o lanza el A* sobre el ruido.
class LegacyFusion : public testing::TestWithParam<Case> {};

TEST_P(LegacyFusion, SameFusionAsTheOriginal) {
  const std::vector<plantraces::Trace> traces = oracle::read_traces(GetParam());
  ASSERT_FALSE(traces.empty());
  const plantraces::Vocabulary vocabulary = plantraces::vocabulary(traces);
  const plantraces::TypeHierarchy hierarchy = plantraces::type_hierarchy(traces);
  // Pocas expansiones: con ruido, el A* original busca el cambio de cada constante alterada, y aquí
  // solo hacen falta reglas con las que probar la fusión.
  datasets::EffectOptions effects = datasets::EffectOptions::original();
  effects.search.max_expansions = 2000;
  std::srand(1);
  for (const auto& [name, transitions] : plantraces::group_by_action(traces)) {
    SCOPED_TRACE("acción " + name);
    const datasets::Table table = datasets::build_table(vocabulary, transitions);
    const std::vector<std::string> types = argument_types(transitions.front());

    const rules::Examples logic = rules::logic_examples(table);
    std::vector<rules::LegacyRule> input = rules::legacy_rules(rules::learn(logic), logic, types);
    const rules::Examples numeric = rules::numeric_examples(
        table, datasets::numeric_effects(table, effects),
        datasets::numeric_relations(table, {}, datasets::RelationOptions::original()),
        [] { return std::rand(); }, rules::ExampleOptions::original());
    const std::vector<rules::LegacyRule> numeric_input =
        rules::legacy_rules(rules::learn(numeric), numeric, types);
    input.insert(input.end(), numeric_input.begin(), numeric_input.end());

    const std::vector<std::string> header =
        hierarchy.actions.contains(name) ? hierarchy.actions.at(name) : std::vector<std::string>{};
    expect_same_fusion(rules::fuse_original(input, header),
                       oracle::legacy_fuse(as_rule_list(input), legacy_header(header)));
  }
}

INSTANTIATE_TEST_SUITE_P(Traces, LegacyFusion,
                         testing::Values(Case{"zenotravel", "complete"},
                                         Case{"zenotravel", "missing30"},
                                         Case{"zenotravel", "noise5"}, Case{"counters", "noise5"}),
                         [](const testing::TestParamInfo<Case>& c) {
                           return oracle::case_name(c.param);
                         });

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
