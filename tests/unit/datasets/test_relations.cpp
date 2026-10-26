#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "datasets/relations.hpp"
#include "traces.hpp"

namespace {

using datasets::AttributeKind;
using datasets::RelationOptions;
using plantraces::LiftedAtom;
using regression::Expression;
using regression::Operation;

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();

// Una tabla con los fluentes (x), (y) y (z), en ese orden, y el instante. Cada columna tiene
// primero las filas de antes y detrás las de después.
datasets::Table table(std::vector<std::vector<double>> fluents) {
  const char* names[] = {"x", "y", "z"};
  datasets::Table table;
  table.transitions = fluents.front().size() / 2;
  for (std::size_t f = 0; f < fluents.size(); ++f) {
    table.attributes.push_back({AttributeKind::fluent, {names[f], {}}});
    table.columns.push_back(std::move(fluents[f]));
  }
  table.attributes.push_back({AttributeKind::time, {}});
  std::vector<double> time;
  for (std::size_t row = 0; row < table.rows(); ++row) {
    time.push_back(static_cast<double>(row));
  }
  table.columns.push_back(std::move(time));
  return table;
}

std::vector<std::string> texts(const std::vector<datasets::Relation>& relations) {
  std::vector<std::string> out;
  for (const datasets::Relation& relation : relations) {
    out.push_back(datasets::to_string(relation));
  }
  return out;
}

const datasets::Relation* find(const std::vector<datasets::Relation>& relations,
                               const std::string& text) {
  for (const datasets::Relation& relation : relations) {
    if (datasets::to_string(relation) == text) {
      return &relation;
    }
  }
  return nullptr;
}

std::vector<datasets::Relation> original(const datasets::Table& t) {
  return datasets::numeric_relations(t, {}, RelationOptions::original());
}

TEST(OriginalRelations, Candidates) {
  // (x) baja 1 y 2, (y) no cambia y (z) sube 2 y 3.
  const datasets::Table t = table({{5, 3, 4, 1}, {4, 4, 4, 4}, {0, 0, 2, 3}});
  const std::vector<datasets::Relation> relations = original(t);

  // (x) con el fluente que va detrás de él (y, z) y con lo que baja (z): -2 y -3, que siempre
  // queda por debajo y se descarta. (y) no cambia: nada, aunque vaya antes que (z). (z) no tiene
  // fluentes detrás, solo lo que baja (x): 1 y 2.
  EXPECT_EQ(texts(relations), (std::vector<std::string>{
                                  "(= (x) (y))",
                                  "(< (x) (y))",
                                  "(> (x) (y))",
                                  "(< (x) (z))",
                                  "(> (x) (z))",
                                  "(< (z) (- (x) (x)'))",
                                  "(> (z) (- (x) (x)'))",
                              }));
  EXPECT_EQ(relations[1].column, (std::vector<double>{0, 1, 0, 1}));
  // Lo que baja (x) en cada aplicación vale lo mismo en su fila de antes y en la de después.
  EXPECT_EQ(relations[5].column, (std::vector<double>{1, 1, 0, 0}));
}

TEST(OriginalRelations, UnknownValuesGiveUnknownRelations) {
  const datasets::Table t = table({{5, unknown, 4, 1}, {4, 4, unknown, 4}, {0, 0, 0, 0}});
  const std::vector<datasets::Relation> relations = original(t);

  ASSERT_FALSE(relations.empty());
  ASSERT_EQ(datasets::to_string(relations[0]), "(< (x) (y))");
  const std::vector<double>& column = relations[0].column;
  EXPECT_EQ(column[0], 0.0);
  EXPECT_TRUE(std::isnan(column[1]));
  EXPECT_TRUE(std::isnan(column[2]));
  EXPECT_EQ(column[3], 1.0);
}

TEST(OriginalRelations, ConstantRelationsAreDropped) {
  // (x) siempre está por debajo de (y) y de (z): cada comparación vale lo mismo en todas las filas.
  const datasets::Table t = table({{1, 2, 0, 1}, {9, 9, 9, 9}, {7, 7, 7, 7}});
  EXPECT_TRUE(original(t).empty());
  EXPECT_TRUE(datasets::numeric_relations(t, {}).empty());
}

TEST(OriginalRelations, NoRelationsWithTheTime) {
  // Solo cambia el instante.
  const datasets::Table t = table({{1, 1, 1, 1}, {2, 2, 2, 2}, {3, 3, 3, 3}});
  EXPECT_TRUE(original(t).empty());
  EXPECT_TRUE(datasets::numeric_relations(t, {}).empty());
}

// Mejora: cada fluente se compara con las expresiones de los efectos, también con la del suyo.
// (x) baja (* (y) (z)): la precondición (>= (x) (* (y) (z))) es (< (x) (* (y) (z))) falsa en los
// estados de antes.
TEST(NumericRelations, ComparesWithTheEffectExpressions) {
  const datasets::Table t = table({{5, 2, 2, 0}, {3, 1, 3, 1}, {1, 2, 1, 2}});
  const LiftedAtom x{"x", {}};
  const std::vector<datasets::FluentChange> changes = {
      {x,
       datasets::NumericEffect{x,
                               datasets::EffectKind::decrease,
                               Expression::binary(Operation::multiply, Expression::variable(1),
                                                  Expression::variable(2)),
                               {x, {"y", {}}, {"z", {}}},
                               0.0},
       {}}};

  const std::vector<datasets::Relation> relations = datasets::numeric_relations(t, changes);
  const datasets::Relation* less = find(relations, "(< (x) (* (y) (z)))");
  ASSERT_NE(less, nullptr) << testing::PrintToString(texts(relations));
  EXPECT_EQ(less->column, (std::vector<double>{0, 0, 1, 1}));
  EXPECT_NE(find(relations, "(= (x) (* (y) (z)))"), nullptr);

  // El original solo compara con lo que bajan los otros fluentes, y no cambia ninguno.
  RelationOptions options = RelationOptions::original();
  EXPECT_EQ(find(datasets::numeric_relations(t, changes, options), "(< (x) (* (y) (z)))"), nullptr);
  options.effect_expressions = true;
  EXPECT_NE(find(datasets::numeric_relations(t, changes, options), "(< (x) (* (y) (z)))"), nullptr);
}

// Las expresiones constantes también: (x) baja 1, y la precondición (>= (x) 1) es (< (x) 1) falsa
// en los estados de antes.
TEST(NumericRelations, ComparesWithConstantEffects) {
  const datasets::Table t = table({{3, 1, 2, 0}, {9, 9, 9, 9}});
  const LiftedAtom x{"x", {}};
  const std::vector<datasets::FluentChange> changes = {
      {x,
       datasets::NumericEffect{
           x, datasets::EffectKind::decrease, Expression::constant(1), {x, {"y", {}}}, 0.0},
       {}}};

  const std::vector<datasets::Relation> relations = datasets::numeric_relations(t, changes);
  const datasets::Relation* less = find(relations, "(< (x) 1)");
  ASSERT_NE(less, nullptr) << testing::PrintToString(texts(relations));
  EXPECT_EQ(less->column, (std::vector<double>{0, 0, 0, 1}));
}

// Mejora: cada pareja en la que cambia algún lado, aunque el que cambia vaya detrás. Es refuel:
// (y) := (x), y (x) va antes en la tabla.
TEST(NumericRelations, PairsInAnyOrder) {
  const datasets::Table t = table({{9, 9, 9, 9}, {3, 5, 9, 9}, {0, 0, 0, 0}});
  EXPECT_EQ(texts(original(t)), std::vector<std::string>{});

  RelationOptions options = RelationOptions::original();
  options.changing_pairs = true;
  EXPECT_EQ(texts(datasets::numeric_relations(t, {}, options)),
            (std::vector<std::string>{"(= (x) (y))", "(> (x) (y))"}));
}

// Mejora: con el modelo nulo, una constante que el ruido altera en una de cada cuatro aplicaciones
// no cambia, y no hay relaciones en las que no cambia nada.
TEST(NumericRelations, NoiseDoesNotMakeConstantsChange) {
  const datasets::Table t = table({{1, 1, 1, 1, 1, 1, 1, 1.5}, {0, 1, 2, 3, 0, 1, 2, 3}});
  EXPECT_FALSE(original(t).empty());

  RelationOptions options = RelationOptions::original();
  options.changing_pairs = true;
  EXPECT_TRUE(datasets::numeric_relations(t, {}, options).empty());
}

// Mejora: las constantes se descartan mirando solo los valores conocidos. El original compara con
// la primera fila aunque sea desconocida (vectorInutil), y entonces se queda con ellas.
TEST(NumericRelations, ConstantsOverTheKnownValues) {
  const datasets::Table t = table({{unknown, 2, 0, 1}, {9, 9, 9, 9}, {7, 7, 7, 7}});
  EXPECT_EQ(texts(original(t)), (std::vector<std::string>{
                                    "(= (x) (y))",
                                    "(< (x) (y))",
                                    "(> (x) (y))",
                                    "(= (x) (z))",
                                    "(< (x) (z))",
                                    "(> (x) (z))",
                                }));

  RelationOptions options = RelationOptions::original();
  options.known_constants = true;
  EXPECT_TRUE(datasets::numeric_relations(t, {}, options).empty());
}

TEST(Relation, Text) {
  const datasets::Relation fall{
      datasets::Comparison::greater,
      {"fuel", {0}},
      {datasets::OperandKind::fall, Expression::variable(0), {{"total-fuel-used", {}}}},
      {}};
  EXPECT_EQ(datasets::to_string(fall), "(> (fuel ?0) (- (total-fuel-used) (total-fuel-used)'))");

  const datasets::Relation fluent{
      datasets::Comparison::less, {"fuel", {0}}, datasets::fluent_operand({"capacity", {0}}), {}};
  EXPECT_EQ(datasets::to_string(fluent), "(< (fuel ?0) (capacity ?0))");
}

// Las precondiciones numéricas de los dominios de referencia de las trazas de prueba
// (vendor/PDDLDomains/dominios/ipc-2023-numeric/<colección>/domain.pddl):
//
//   zenotravel  fly-slow:  (>= (fuel ?a) (* (distance ?c1 ?c2) (slow-burn ?a)))
//               fly-fast:  (>= (fuel ?a) (* (distance ?c1 ?c2) (fast-burn ?a)))
//                          (<= (onboard ?a) (zoom-limit ?a))
//               refuel:    (> (capacity ?a) (fuel ?a))
//   counters    increment: (<= (+ (value ?c) 1) (max_int))
//               decrement: (>= (value ?c) 1)
//
// Una se puede expresar si hay una relación que compara las mismas dos magnitudes: sus valores
// coinciden en las trazas completas (así vale (distance ?2 ?1) por (distance ?1 ?2)). La
// comparación concreta la eligen después las reglas.
//
// Dos no se pueden aprender con estas trazas, porque no se incumplen ni se alcanzan en ningún
// estado y son indistinguibles de un invariante del dominio: (<= (onboard ?a) (zoom-limit ?a)) y
// la de increment (ningún contador llega a max_int). No están.
struct Precondition {
  std::string collection;
  std::string action;
  LiftedAtom fluent;
  datasets::Operand other;
};

std::vector<Precondition> reference(const std::string& collection) {
  const auto consumption = [](const char* burn) {
    return datasets::Operand{
        datasets::OperandKind::expression,
        Expression::binary(Operation::multiply, Expression::variable(0), Expression::variable(1)),
        {{"distance", {1, 2}}, {burn, {0}}}};
  };
  const std::vector<Precondition> all = {
      {"zenotravel", "fly-slow", {"fuel", {0}}, consumption("slow-burn")},
      {"zenotravel", "fly-fast", {"fuel", {0}}, consumption("fast-burn")},
      {"zenotravel", "refuel", {"capacity", {0}}, datasets::fluent_operand({"fuel", {0}})},
      {"counters",
       "decrement",
       {"value", {0}},
       {datasets::OperandKind::expression, Expression::constant(1), {}}},
  };
  std::vector<Precondition> out;
  std::copy_if(all.begin(), all.end(), std::back_inserter(out),
               [&](const Precondition& p) { return p.collection == collection; });
  return out;
}

bool same_values(const std::vector<double>& a, const std::vector<double>& b) {
  for (std::size_t i = 0; i < a.size(); ++i) {
    if (std::isnan(a[i]) != std::isnan(b[i]) ||
        (!std::isnan(a[i]) && std::abs(a[i] - b[i]) > 1e-9 * std::max(1.0, std::abs(a[i])))) {
      return false;
    }
  }
  return true;
}

// Cuántas precondiciones de referencia de la colección se pueden expresar con las relaciones de
// cada acción en una variante.
std::size_t expressed(const std::string& collection, const std::string& variant,
                      const RelationOptions& options) {
  const datasets::test::Tables tables(collection, variant);
  const datasets::test::Tables complete(collection, "complete");
  std::size_t found = 0;
  for (const Precondition& precondition : reference(collection)) {
    SCOPED_TRACE(precondition.action + " " + plantraces::to_string(precondition.fluent) + " " +
                 datasets::to_string(precondition.other));
    const datasets::Table& table = tables.at(precondition.action);
    const datasets::Table& clean = complete.at(precondition.action);
    const std::vector<double> fluent =
        datasets::values(clean, datasets::fluent_operand(precondition.fluent));
    const std::vector<double> other = datasets::values(clean, precondition.other);

    const std::vector<datasets::Relation> relations =
        datasets::numeric_relations(table, datasets::numeric_effects(table), options);
    const bool any =
        std::any_of(relations.begin(), relations.end(), [&](const datasets::Relation& relation) {
          const std::vector<double> left =
              datasets::values(clean, datasets::fluent_operand(relation.left));
          const std::vector<double> right = datasets::values(clean, relation.right);
          return (same_values(left, fluent) && same_values(right, other)) ||
                 (same_values(left, other) && same_values(right, fluent));
        });
    found += any ? 1U : 0U;
  }
  return found;
}

struct Case {
  std::string collection;
  std::string variant;
};

class ReferencePreconditions : public testing::TestWithParam<Case> {};

TEST_P(ReferencePreconditions, AreExpressed) {
  EXPECT_EQ(expressed(GetParam().collection, GetParam().variant, {}),
            reference(GetParam().collection).size());
}

INSTANTIATE_TEST_SUITE_P(Traces, ReferencePreconditions,
                         testing::Values(Case{"zenotravel", "complete"},
                                         Case{"zenotravel", "noise5"},
                                         Case{"zenotravel", "missing30"},
                                         Case{"counters", "complete"}, Case{"counters", "noise5"},
                                         Case{"counters", "missing30"}),
                         [](const testing::TestParamInfo<Case>& c) {
                           return c.param.collection + "_" + c.param.variant;
                         });

// El original no puede expresar ninguna. En zenotravel compara fuel con lo que bajan los otros
// fluentes y no con lo que baja él, y en refuel capacity va antes que fuel en la tabla y no cambia.
// En counters no compara con constantes.
TEST(ReferencePreconditions, TheOriginalExpressesNone) {
  EXPECT_EQ(expressed("zenotravel", "complete", RelationOptions::original()), 0U);
  EXPECT_EQ(expressed("counters", "complete", RelationOptions::original()), 0U);
}

}  // namespace
