// Los ejemplos de INSLV y la interpretación de sus reglas. Que coincidan con el original lo
// comprueba el oráculo (tests/oracle/test_legacy_rules.cpp); aquí, casos pequeños.

#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "rules/examples.hpp"

namespace {

using datasets::AttributeKind;
using regression::Expression;

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();

int first() { return 0; }

// Dos aplicaciones (cuatro filas) con el átomo (p), los fluentes (x) e (y) y el instante.
datasets::Table table(std::vector<double> p, std::vector<double> x, std::vector<double> y) {
  datasets::Table table;
  table.transitions = 2;
  table.attributes = {{AttributeKind::atom, {"p", {}}},
                      {AttributeKind::fluent, {"x", {}}},
                      {AttributeKind::fluent, {"y", {}}},
                      {AttributeKind::time, {}}};
  table.columns = {std::move(p), std::move(x), std::move(y), {0, 2, 1, 3}};
  return table;
}

std::vector<std::string> names(const rules::Examples& examples) {
  std::vector<std::string> out;
  for (const inslv::Variable& variable : examples.data.attributes) {
    out.push_back(variable.name);
  }
  return out;
}

std::vector<std::string> labels(const inslv::Variable& variable) {
  std::vector<std::string> out;
  for (const inslv::FuzzySet& set : variable.labels) {
    out.push_back(set.label);
  }
  return out;
}

TEST(LogicExamples, TheAtoms) {
  const rules::Examples examples = rules::logic_examples(table({1, 1, 0, unknown}, {}, {}));
  EXPECT_EQ(names(examples), std::vector<std::string>{"(p)"});
  EXPECT_EQ(labels(examples.data.attributes[0]), (std::vector<std::string>{"False", "True"}));
  ASSERT_EQ(examples.data.values.size(), 1U);
  EXPECT_EQ(examples.data.values[0][2], 0.0);
  EXPECT_TRUE(std::isnan(examples.data.values[0][3]));
  EXPECT_EQ(examples.data.classes, (std::vector<std::size_t>{0, 0, 1, 1}));
  EXPECT_EQ(labels(examples.data.target), (std::vector<std::string>{"PreState", "PostState"}));
  EXPECT_EQ(examples.attributes[0].kind, rules::AttributeKind::atom);
}

// Los valores de los fluentes, las relaciones y los cambios. (x) sube 1 en las dos aplicaciones e
// (y) no cambia: sin cambio. Con el instante, como el original, también su valor y su cambio.
TEST(NumericExamples, ValuesRelationsAndChanges) {
  const datasets::Table t = table({1, 1, 1, 1}, {3, 5, 4, 6}, {9, 9, 9, 9});
  const plantraces::LiftedAtom x{"x", {}};
  const std::vector<datasets::FluentChange> changes = {
      {x,
       datasets::NumericEffect{x, datasets::EffectKind::magnitude, Expression::constant(1), {}, 0},
       {}}};
  datasets::Relation relation{datasets::Comparison::less,
                              {AttributeKind::fluent, x},
                              datasets::fluent_operand({"y", {}}),
                              {1, 1, 1, 1}};

  const rules::Examples examples =
      rules::numeric_examples(t, changes, {relation}, first, rules::ExampleOptions::original());
  EXPECT_EQ(names(examples),
            (std::vector<std::string>{"value (x)", "value (y)", "value time", "(< (x) (y))",
                                      "change (x) 1", "change time 1"}));
  EXPECT_EQ(labels(examples.data.attributes[3]), (std::vector<std::string>{"False", "True"}));
  EXPECT_EQ(labels(examples.data.attributes[4]), (std::vector<std::string>{"NA", "POS"}));
  EXPECT_EQ(examples.data.values[4], (std::vector<double>{0, 0, 1, 1}));
  EXPECT_EQ(examples.attributes[4].kind, rules::AttributeKind::change);

  // Sin el instante.
  EXPECT_EQ(names(rules::numeric_examples(t, changes, {relation}, first)),
            (std::vector<std::string>{"value (x)", "value (y)", "(< (x) (y))", "change (x) 1"}));
}

// El cambio vale -1 en las filas de después si el valor no sube, también si no cambia, como en el
// original; desconocido si falta el valor. Y sin expresión no hay atributo del cambio.
TEST(NumericExamples, TheChangeSign) {
  const datasets::Table t = table({1, 1, 1, 1}, {3, 5, 2, unknown}, {4, 4, 4, 5});
  const plantraces::LiftedAtom x{"x", {}};
  const std::vector<datasets::FluentChange> changes = {
      {x,
       datasets::NumericEffect{x, datasets::EffectKind::magnitude, Expression::constant(1), {}, 0},
       {}},
      {{"y", {}}, std::nullopt, {}}};

  const rules::Examples examples = rules::numeric_examples(t, changes, {}, first);
  EXPECT_EQ(names(examples), (std::vector<std::string>{"value (x)", "value (y)", "change (x) 1"}));
  const std::vector<double>& change = examples.data.values[2];
  EXPECT_EQ(change[0], 0.0);
  EXPECT_EQ(change[1], 0.0);
  EXPECT_EQ(change[2], -1.0);
  EXPECT_TRUE(std::isnan(change[3]));
  EXPECT_EQ(labels(examples.data.attributes[2]), (std::vector<std::string>{"NA", "NEG"}));
}

// Un átomo cierto antes y falso después: la regla del estado de antes pide (p) y la del de
// después, (not (p)).
TEST(Learn, InterpretsTheRules) {
  datasets::Table t;
  t.transitions = 4;
  t.attributes = {{AttributeKind::atom, {"p", {}}}};
  t.columns = {{1, 1, 1, 1, 0, 0, 0, 0}};
  const std::vector<rules::Rule> learned = rules::learn(rules::logic_examples(t));

  ASSERT_EQ(learned.size(), 2U);
  for (const rules::Rule& rule : learned) {
    EXPECT_EQ(rule.covered, 4);
    EXPECT_EQ(rule.conditions, (std::vector<rules::Condition>{{0, !rule.post}}));
  }
}

TEST(Learn, NoAttributesNoRules) {
  datasets::Table t;
  t.transitions = 2;
  t.attributes = {{AttributeKind::time, {}}};
  t.columns = {{0, 2, 1, 3}};
  EXPECT_TRUE(rules::learn(rules::logic_examples(t)).empty());
}

}  // namespace
