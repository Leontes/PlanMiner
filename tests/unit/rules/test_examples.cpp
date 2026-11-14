// Los ejemplos de INSLV y la interpretación de sus reglas. Que coincidan con el original lo
// comprueba el oráculo (tests/oracle/test_legacy_rules.cpp); aquí, casos pequeños.

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "plantraces/trace.hpp"
#include "rules/examples.hpp"
#include "rules/legacy.hpp"

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
  for (const inslv::Variable& variable : examples.data.atributos) {
    out.push_back(variable.nombre);
  }
  return out;
}

std::vector<std::string> labels(const inslv::Variable& variable) {
  std::vector<std::string> out;
  for (const inslv::ConjuntoDifuso& set : variable.etiquetas) {
    out.push_back(set.etiqueta);
  }
  return out;
}

TEST(LogicExamples, TheAtoms) {
  const rules::Examples examples = rules::logic_examples(table({1, 1, 0, unknown}, {}, {}));
  EXPECT_EQ(names(examples), std::vector<std::string>{"(p)"});
  EXPECT_EQ(labels(examples.data.atributos[0]), (std::vector<std::string>{"False", "True"}));
  ASSERT_EQ(examples.data.valores.size(), 1U);
  EXPECT_EQ(examples.data.valores[0][2], 0.0);
  EXPECT_TRUE(std::isnan(examples.data.valores[0][3]));
  EXPECT_EQ(examples.data.clases, (std::vector<std::size_t>{0, 0, 1, 1}));
  EXPECT_EQ(labels(examples.data.clase), (std::vector<std::string>{"PreState", "PostState"}));
  EXPECT_EQ(examples.attributes[0].kind, rules::AttributeKind::atom);
}

// Los valores de los fluentes, las relaciones y los cambios. (x) sube 1 en las dos aplicaciones e
// (y) no cambia: sin cambio. Con el instante, como el original, también su valor y su cambio.
// lowFreq: en cada clase, el valor que tiene menos de threshold de las filas en que se conoce.
TEST(HideRareValues, TheRareValueOfEachClass) {
  datasets::Table t;
  t.transitions = 20;
  t.attributes = {{AttributeKind::atom, {"p", {}}}};
  // Antes: 18 ciertas, 1 falsa y 1 desconocida. Después: todas falsas.
  std::vector<double> p(40, 0.0);
  std::fill(p.begin(), p.begin() + 20, 1.0);
  p[3] = 0.0;
  p[4] = unknown;
  t.columns = {p};

  // 1 de 19 es más del 5 %.
  rules::Examples examples = rules::logic_examples(t);
  rules::hide_rare_values(examples, 0.05);
  EXPECT_EQ(examples.data.valores[0][3], 0.0);

  examples = rules::logic_examples(t);
  rules::hide_rare_values(examples, 0.06);
  EXPECT_TRUE(std::isnan(examples.data.valores[0][3]));
  EXPECT_EQ(examples.data.valores[0][0], 1.0);
  EXPECT_TRUE(std::isnan(examples.data.valores[0][4]));
  // Después, el único valor es el de todas las filas.
  for (std::size_t row = 20; row < 40; ++row) {
    EXPECT_EQ(examples.data.valores[0][row], 0.0);
  }
}

// Con 100 filas, un 5 % de ruido y alpha 1e-4, lo que sale en 15 filas o menos. Antes: (p) falso en
// 15 filas, (q) falso en 16 y (r) cierto en 50 (la mitad no se borra nunca).
TEST(HideRareValuesBinomial, WhatTheNoiseExplains) {
  datasets::Table t;
  t.transitions = 100;
  t.attributes = {{AttributeKind::atom, {"p", {}}},
                  {AttributeKind::atom, {"q", {}}},
                  {AttributeKind::atom, {"r", {}}}};
  std::vector<double> p(200, 1.0);
  std::vector<double> q(200, 1.0);
  std::vector<double> r(200, 0.0);
  std::fill(p.begin(), p.begin() + 15, 0.0);
  std::fill(q.begin(), q.begin() + 16, 0.0);
  std::fill(r.begin(), r.begin() + 50, 1.0);
  t.columns = {p, q, r};

  rules::Examples examples = rules::logic_examples(t);
  rules::hide_rare_values_binomial(examples, 0.05, 1e-4);
  const std::vector<std::vector<double>>& values = examples.data.valores;
  EXPECT_TRUE(std::isnan(values[0][0]));
  EXPECT_EQ(values[0][15], 1.0);
  EXPECT_EQ(values[1][0], 0.0);
  EXPECT_EQ(values[2][0], 1.0);
  EXPECT_EQ(values[2][50], 0.0);
}

// Con el ruido de cada columna: (p) y (q), falsos en 10 de las 100 filas de antes y de las 100 de
// después. Con un 5 % de ruido y alpha 1e-4, la cota es 15, y se borran; en (q) después, con un
// 0.1 % (como el que queda al suavizar los átomos), es 3, y se quedan.
TEST(HideRareValuesBinomial, TheNoiseOfEachColumn) {
  datasets::Table t;
  t.transitions = 100;
  t.attributes = {{AttributeKind::atom, {"p", {}}}, {AttributeKind::atom, {"q", {}}}};
  std::vector<double> column(200, 1.0);
  std::fill(column.begin(), column.begin() + 10, 0.0);
  std::fill(column.begin() + 100, column.begin() + 110, 0.0);
  t.columns = {column, column};

  rules::Examples examples = rules::logic_examples(t);
  rules::hide_rare_values_binomial(
      examples,
      [](std::size_t attribute, std::size_t klass) {
        return attribute == 1 && klass == 1 ? 0.001 : 0.05;
      },
      1e-4);
  const std::vector<std::vector<double>>& values = examples.data.valores;
  EXPECT_TRUE(std::isnan(values[0][0]));
  EXPECT_TRUE(std::isnan(values[0][100]));
  EXPECT_TRUE(std::isnan(values[1][0]));
  EXPECT_EQ(values[1][100], 0.0);
}

// Un fluente sin valores conocidos no tiene etiquetas: no se le pasa a INSLV.
TEST(NumericExamples, AFluentWithoutValues) {
  const rules::Examples examples = rules::numeric_examples(
      table({1, 1, 1, 1}, {unknown, unknown, unknown, unknown}, {9, 9, 9, 9}), {}, {}, first);
  EXPECT_EQ(names(examples), std::vector<std::string>{"value (y)"});
}

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
  EXPECT_EQ(labels(examples.data.atributos[3]), (std::vector<std::string>{"False", "True"}));
  EXPECT_EQ(labels(examples.data.atributos[4]), (std::vector<std::string>{"NA", "POS"}));
  EXPECT_EQ(examples.data.valores[4], (std::vector<double>{0, 0, 1, 1}));
  EXPECT_EQ(examples.attributes[4].kind, rules::AttributeKind::change);

  // Sin el instante.
  rules::ExampleOptions options;
  options.changes = true;
  EXPECT_EQ(names(rules::numeric_examples(t, changes, {relation}, first, options)),
            (std::vector<std::string>{"value (x)", "value (y)", "(< (x) (y))", "change (x) 1"}));

  // Sin los cambios: los efectos numéricos salen de la regresión.
  EXPECT_EQ(names(rules::numeric_examples(t, changes, {relation}, first)),
            (std::vector<std::string>{"value (x)", "value (y)", "(< (x) (y))"}));
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

  rules::ExampleOptions options;
  options.changes = true;
  const rules::Examples examples = rules::numeric_examples(t, changes, {}, first, options);
  EXPECT_EQ(names(examples), (std::vector<std::string>{"value (x)", "value (y)", "change (x) 1"}));
  const std::vector<double>& change = examples.data.valores[2];
  EXPECT_EQ(change[0], 0.0);
  EXPECT_EQ(change[1], 0.0);
  EXPECT_EQ(change[2], -1.0);
  EXPECT_TRUE(std::isnan(change[3]));
  EXPECT_EQ(labels(examples.data.atributos[2]), (std::vector<std::string>{"NA", "NEG"}));
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

// La regla de las cotas: (<= (onboard ?0) (zoom ?0)) se cumple en los dos estados de antes, se
// alcanza en uno y no se cumple en todos los estados de la traza (p1 lleva 2 con zoom 1). (<=
// (onboard ?0) (cap ?0)) deja siempre holgura, y (>= (zoom ?0) 0) es un invariante.
TEST(ContrastRule, ReachedBoundsThatAreNotInvariants) {
  const std::vector<plantraces::Trace> traces = {plantraces::parse_trace(R"json({
    "format": "pddldomains-trace", "version": 1, "domain": {"name": "d"},
    "problem": {"name": "p"}, "world": "closed", "objects": {"p1": "plane", "p2": "plane"},
    "states": [
      {"step": 0, "time": 0, "true": [], "unknown": [],
       "values": {"(onboard p1)": 0, "(zoom p1)": 1, "(cap p1)": 9}, "unknown_values": []},
      {"step": 1, "time": 1, "true": [], "unknown": [],
       "values": {"(onboard p1)": 2, "(zoom p1)": 1, "(cap p1)": 9}, "unknown_values": []}
    ],
    "actions": [{"step": 1, "time": 1, "name": "board", "arguments": ["p1"]}]
  })json")};
  const datasets::Attribute onboard{datasets::AttributeKind::fluent, {"onboard", {0}}};
  const datasets::Attribute zoom{datasets::AttributeKind::fluent, {"zoom", {0}}};
  // Dos aplicaciones: onboard 0 y 1 antes (zoom 1, cap 9), igual después.
  const auto relation = [](datasets::Comparison comparison, const datasets::Attribute& left,
                           datasets::Operand right, std::vector<double> column) {
    return datasets::Relation{comparison, left, std::move(right), std::move(column)};
  };
  using datasets::Comparison;
  const datasets::Operand zoom_operand = datasets::fluent_operand(zoom.term);
  const datasets::Operand cap = datasets::fluent_operand({"cap", {0}});
  const datasets::Operand zero{
      datasets::OperandKind::expression, regression::Expression::constant(0), {}};
  const std::vector<datasets::Relation> candidates = {
      relation(Comparison::equal, onboard, zoom_operand, {0, 1, 0, 1}),
      relation(Comparison::less, onboard, zoom_operand, {1, 0, 1, 0}),
      relation(Comparison::greater, onboard, zoom_operand, {0, 0, 0, 0}),
      relation(Comparison::equal, onboard, cap, {0, 0, 0, 0}),
      relation(Comparison::less, onboard, cap, {1, 1, 1, 1}),
      relation(Comparison::greater, onboard, cap, {0, 0, 0, 0}),
      relation(Comparison::equal, zoom, zero, {0, 0, 0, 0}),
      relation(Comparison::less, zoom, zero, {0, 0, 0, 0}),
      relation(Comparison::greater, zoom, zero, {1, 1, 1, 1}),
  };
  const rules::LegacyRule rule = rules::contrast_rule(candidates, {}, {"plane"}, traces, 1.0);
  EXPECT_EQ(rule.covered, -2);
  datasets::Relation greater = candidates[2];
  greater.column.clear();
  const std::string name = rules::legacy_name(
      {rules::AttributeKind::relation, onboard, greater, std::nullopt, {}}, {}, {"plane"});
  EXPECT_EQ(rule.conditions, (std::vector<rules::Literal>{{name, "False"}}));
}

TEST(Learn, NoAttributesNoRules) {
  datasets::Table t;
  t.transitions = 2;
  t.attributes = {{AttributeKind::time, {}}};
  t.columns = {{0, 2, 1, 3}};
  EXPECT_TRUE(rules::learn(rules::logic_examples(t)).empty());
}

}  // namespace
