// Los efectos numéricos de zenotravel (tests/data/traces/zenotravel): los del dominio de
// referencia (vendor/PDDLDomains/dominios/ipc-2023-numeric/zenotravel/domain.pddl) son
//
//   board:    (increase (onboard ?a) 1)
//   debark:   (decrease (onboard ?a) 1)
//   fly-slow: (decrease (fuel ?a) (* (distance ?c1 ?c2) (slow-burn ?a)))
//             (increase (total-fuel-used) (* (distance ?c1 ?c2) (slow-burn ?a)))
//   fly-fast: lo mismo con fast-burn
//   refuel:   (assign (fuel ?a) (capacity ?a))
//
// Una expresión encontrada es correcta si da los mismos valores que la de referencia en las
// trazas completas: en zenotravel las distancias son simétricas y (distance ?1 ?2) y
// (distance ?2 ?1) son igual de buenas.

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "datasets/effects.hpp"

namespace {

namespace fs = std::filesystem;

using datasets::EffectKind;
using plantraces::LiftedAtom;
using regression::Expression;
using regression::Operation;

std::vector<plantraces::Trace> zenotravel(const std::string& variant) {
  std::vector<fs::path> files;
  for (const auto& entry : fs::directory_iterator(fs::path{PLANMINER_TRACES_DIR} / "zenotravel")) {
    if (entry.path().filename().string().ends_with("." + variant + ".json")) {
      files.push_back(entry.path());
    }
  }
  std::sort(files.begin(), files.end());
  std::vector<plantraces::Trace> traces;
  for (const fs::path& file : files) {
    traces.push_back(plantraces::read_trace(file));
  }
  return traces;
}

// Las tablas de las acciones de una variante.
class Tables {
 public:
  explicit Tables(const std::string& variant) : traces_(zenotravel(variant)) {
    const plantraces::Vocabulary vocabulary = plantraces::vocabulary(traces_);
    for (const auto& [name, transitions] : plantraces::group_by_action(traces_)) {
      tables_.emplace(name, datasets::build_table(vocabulary, transitions));
    }
  }
  const datasets::Table& at(const std::string& action) const { return tables_.at(action); }
  const std::map<std::string, datasets::Table>& all() const { return tables_; }

 private:
  std::vector<plantraces::Trace> traces_;
  std::map<std::string, datasets::Table> tables_;
};

// Los valores de la expresión (sobre los fluentes variables) en los estados de antes de la tabla.
std::vector<double> evaluate(const datasets::Table& table, const std::vector<LiftedAtom>& variables,
                             const Expression& expression) {
  std::vector<std::vector<double>> columns;
  for (const LiftedAtom& variable : variables) {
    const auto found = std::find(table.attributes.begin(), table.attributes.end(),
                                 datasets::Attribute{datasets::AttributeKind::fluent, variable});
    EXPECT_NE(found, table.attributes.end()) << plantraces::to_string(variable);
    const std::vector<double>& column =
        table.columns[static_cast<std::size_t>(found - table.attributes.begin())];
    columns.emplace_back(column.begin(),
                         column.begin() + static_cast<std::ptrdiff_t>(table.transitions));
  }
  return expression.evaluate(columns, table.transitions);
}

struct Expected {
  std::string action;
  LiftedAtom fluent;
  EffectKind kind;
  std::vector<LiftedAtom> variables;
  Expression expression;
};

const LiftedAtom distance{"distance", {1, 2}};

Expression product() {
  return Expression::binary(Operation::multiply, Expression::variable(0), Expression::variable(1));
}

std::vector<Expected> reference() {
  return {
      {"board", {"onboard", {1}}, EffectKind::increase, {}, Expression::constant(1)},
      {"debark", {"onboard", {1}}, EffectKind::decrease, {}, Expression::constant(1)},
      {"fly-slow", {"fuel", {0}}, EffectKind::decrease, {distance, {"slow-burn", {0}}}, product()},
      {"fly-slow",
       {"total-fuel-used", {}},
       EffectKind::increase,
       {distance, {"slow-burn", {0}}},
       product()},
      {"fly-fast", {"fuel", {0}}, EffectKind::decrease, {distance, {"fast-burn", {0}}}, product()},
      {"fly-fast",
       {"total-fuel-used", {}},
       EffectKind::increase,
       {distance, {"fast-burn", {0}}},
       product()},
      {"refuel", {"fuel", {0}}, EffectKind::assign, {{"capacity", {0}}}, Expression::variable(0)},
  };
}

class ZenotravelEffects : public testing::TestWithParam<std::string> {};

TEST_P(ZenotravelEffects, FindsTheEffectsOfTheReferenceDomainAndNoOthers) {
  const Tables tables(GetParam());
  const Tables complete("complete");

  const std::vector<Expected> expected_effects = reference();
  std::size_t found = 0;
  for (const auto& [action, table] : tables.all()) {
    for (const datasets::FluentChange& change : datasets::numeric_effects(table)) {
      SCOPED_TRACE(action + " " + plantraces::to_string(change.fluent));
      const auto expected = std::find_if(
          expected_effects.begin(), expected_effects.end(), [&](const Expected& candidate) {
            return candidate.action == action && candidate.fluent == change.fluent;
          });
      if (expected == expected_effects.end()) {
        ADD_FAILURE() << "efecto que no está en el dominio: "
                      << (change.effect ? datasets::to_string(*change.effect) : "(sin expresión)");
        continue;
      }
      ASSERT_TRUE(change.effect) << "sin expresión";
      ++found;
      EXPECT_EQ(change.effect->kind, expected->kind) << datasets::to_string(*change.effect);

      const datasets::Table& clean = complete.at(action);
      const std::vector<double> learned =
          evaluate(clean, change.effect->variables, change.effect->expression);
      const std::vector<double> truth = evaluate(clean, expected->variables, expected->expression);
      for (std::size_t i = 0; i < learned.size(); ++i) {
        EXPECT_NEAR(learned[i], truth[i], 1e-9)
            << datasets::to_string(*change.effect) << ", aplicación " << i;
      }
    }
  }
  EXPECT_EQ(found, expected_effects.size());
}

INSTANTIATE_TEST_SUITE_P(Variants, ZenotravelEffects,
                         testing::Values("complete", "noise5", "missing30"));

// La formulación original no puede expresar refuel: fuel := capacity necesita el fuel de antes.
// Con las variables del estado de antes, sí, aunque el resto sea como en el original.
TEST(ZenotravelEffects, RefuelNeedsTheStateBefore) {
  const Tables complete("complete");
  const datasets::Table& refuel = complete.at("refuel");

  datasets::EffectOptions original = datasets::EffectOptions::original();
  original.search.max_expansions = 2000;  // no hay expresión: que no tarde
  const std::vector<datasets::FluentChange> without = datasets::numeric_effects(refuel, original);
  ASSERT_EQ(without.size(), 1U);
  EXPECT_FALSE(without[0].effect);
  EXPECT_TRUE(without[0].search.limit_reached);

  original.before_state = true;
  const std::vector<datasets::FluentChange> with = datasets::numeric_effects(refuel, original);
  ASSERT_EQ(with.size(), 1U);
  ASSERT_TRUE(with[0].effect);
  EXPECT_EQ(datasets::to_string(*with[0].effect),
            "(magnitude (fuel ?0) (- (capacity ?0) (fuel ?0)))");
}

TEST(NumericEffect, Text) {
  const datasets::NumericEffect effect{
      {"fuel", {0}}, EffectKind::decrease, product(), {distance, {"slow-burn", {0}}}, 0.0};
  EXPECT_EQ(datasets::to_string(effect),
            "(decrease (fuel ?0) (* (distance ?1 ?2) (slow-burn ?0)))");
}

}  // namespace
