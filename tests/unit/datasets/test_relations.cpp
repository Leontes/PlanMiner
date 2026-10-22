#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "datasets/relations.hpp"

namespace {

using datasets::Attribute;
using datasets::AttributeKind;

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();

// Una tabla de dos aplicaciones (cuatro filas: dos de antes y dos de después) con los fluentes
// (x), (y) y (z), en ese orden, y el instante.
datasets::Table table(std::vector<double> x, std::vector<double> y, std::vector<double> z) {
  datasets::Table table;
  table.transitions = 2;
  table.attributes = {{AttributeKind::fluent, {"x", {}}},
                      {AttributeKind::fluent, {"y", {}}},
                      {AttributeKind::fluent, {"z", {}}},
                      {AttributeKind::time, {}}};
  table.columns = {std::move(x), std::move(y), std::move(z), {0, 2, 1, 3}};
  return table;
}

std::vector<std::string> texts(const std::vector<datasets::Relation>& relations) {
  std::vector<std::string> out;
  for (const datasets::Relation& relation : relations) {
    out.push_back(datasets::to_string(relation));
  }
  return out;
}

TEST(NumericRelations, TheOriginalCandidates) {
  // (x) baja 1 y 2, (y) no cambia y (z) sube 2 y 3.
  const datasets::Table t = table({5, 3, 4, 1}, {4, 4, 4, 4}, {0, 0, 2, 3});
  const std::vector<datasets::Relation> relations = datasets::numeric_relations(t);

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

TEST(NumericRelations, UnknownValuesGiveUnknownRelations) {
  const datasets::Table t = table({5, unknown, 4, 1}, {4, 4, unknown, 4}, {0, 0, 0, 0});
  const std::vector<datasets::Relation> relations = datasets::numeric_relations(t);

  ASSERT_FALSE(relations.empty());
  ASSERT_EQ(datasets::to_string(relations[0]), "(< (x) (y))");
  const std::vector<double>& column = relations[0].column;
  EXPECT_EQ(column[0], 0.0);
  EXPECT_TRUE(std::isnan(column[1]));
  EXPECT_TRUE(std::isnan(column[2]));
  EXPECT_EQ(column[3], 1.0);
}

TEST(NumericRelations, ConstantRelationsAreDropped) {
  // (x) siempre está por debajo de (y) y de (z): cada comparación vale lo mismo en todas las filas.
  const datasets::Table t = table({1, 2, 0, 1}, {9, 9, 9, 9}, {7, 7, 7, 7});
  EXPECT_TRUE(datasets::numeric_relations(t).empty());
}

// Como en el original (vectorInutil), una relación que no se conoce en la primera fila se queda
// aunque valga lo mismo en todas las demás.
TEST(NumericRelations, AnUnknownFirstRowKeepsTheRelation) {
  const datasets::Table t = table({unknown, 2, 0, 1}, {9, 9, 9, 9}, {7, 7, 7, 7});
  EXPECT_EQ(texts(datasets::numeric_relations(t)), (std::vector<std::string>{
                                                       "(= (x) (y))",
                                                       "(< (x) (y))",
                                                       "(> (x) (y))",
                                                       "(= (x) (z))",
                                                       "(< (x) (z))",
                                                       "(> (x) (z))",
                                                   }));
}

TEST(NumericRelations, NoRelationsWithTheTime) {
  // Solo cambia el instante.
  const datasets::Table t = table({1, 1, 1, 1}, {2, 2, 2, 2}, {3, 3, 3, 3});
  EXPECT_TRUE(datasets::numeric_relations(t).empty());
}

TEST(Relation, Text) {
  const datasets::Relation relation{datasets::Comparison::greater,
                                    {"fuel", {0}},
                                    datasets::OperandKind::fall,
                                    {"total-fuel-used", {}},
                                    {}};
  EXPECT_EQ(datasets::to_string(relation),
            "(> (fuel ?0) (- (total-fuel-used) (total-fuel-used)'))");
}

}  // namespace
