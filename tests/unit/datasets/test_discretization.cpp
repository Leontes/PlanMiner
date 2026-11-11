// La discretización del original. Que dé los mismos números que el original lo comprueba el
// oráculo (tests/oracle/test_legacy_discretization.cpp); aquí, su comportamiento en casos pequeños.

#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <limits>
#include <vector>

#include "datasets/discretization.hpp"

namespace {

using datasets::FuzzySet;

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();

// Un generador que no se debería usar.
int unused() {
  ADD_FAILURE() << "se ha resembrado un cluster";
  return 0;
}

// Siempre el primer valor: random() / (RAND_MAX / n) es 0.
int first() { return 0; }

// Con k = 2 los dos centroides empiezan en el mismo valor, el segundo se queda vacío y se resiembra
// con otro igual: el índice no mejora y queda uno.
TEST(Discretize, AConstantIsOneCrispSet) {
  EXPECT_EQ(datasets::discretize({4, 4, unknown, 4}, first), (std::vector<FuzzySet>{{4, 4, 4, 4}}));
}

TEST(Discretize, NoKnownValuesNoSets) {
  EXPECT_TRUE(datasets::discretize({}, unused).empty());
  EXPECT_TRUE(datasets::discretize({unknown, unknown}, unused).empty());
}

// Dos grupos separados: dos trapecios, cada uno con su grupo en el soporte. El soporte se pasa un
// poco de los valores (1/500 y 1/100 de una barra del histograma).
TEST(Discretize, TwoGroupsTwoTrapezoids) {
  const std::vector<double> values = {1, 2, 3, 2, 1, 101, 102, 103, 102, 101};
  const std::vector<FuzzySet> sets = datasets::discretize(values, unused);
  ASSERT_EQ(sets.size(), 2U);
  for (const FuzzySet& set : sets) {
    EXPECT_LE(set.a, set.b);
    EXPECT_LE(set.b, set.c);
    EXPECT_LE(set.c, set.d);
  }
  // En el orden de k-means: los centroides empiezan en values[2] y values[7].
  EXPECT_LT(sets[0].a, 1.0);
  EXPECT_GT(sets[0].d, 3.0);
  EXPECT_LT(sets[0].d, 101.0);
  EXPECT_GT(sets[1].a, 3.0);
  EXPECT_LT(sets[1].a, 101.0);
  EXPECT_GT(sets[1].d, 103.0);
}

// Con k = 2, los dos centroides empiezan en el mismo valor (values[1] y values[3]), todo cae en el
// primero y el segundo se resiembra con un valor al azar: random() / (RAND_MAX / 5), un índice, el
// 0. El primero se va a 2,6 (la media), así que en la iteración siguiente los 1 pasan al segundo y
// el 9 se queda solo. Los clusters salen en el orden de k-means, no por valor.
TEST(Discretize, EmptyClustersAreReseeded) {
  const std::vector<double> values = {1, 1, 1, 1, 9};
  std::size_t calls = 0;
  const std::vector<FuzzySet> sets = datasets::discretize(values, [&calls] {
    ++calls;
    return 0;
  });
  EXPECT_GT(calls, 0U);
  EXPECT_EQ(sets, (std::vector<FuzzySet>{{9, 9, 9, 9}, {1, 1, 1, 1}}));
}

// La columna (bat_usage ?arg1) de la tabla 2 del artículo de PlanMiner-N (figura 2): los valores
// cerca de 3 y de 5 quedan en dos clusters y pasan a su media; el atípico, -4, a desconocido.
const std::vector<double> bat_usage{3.25, 3, 3, 3, 3, 3, 2.97, 3, 5, 5.05, 5, 5, 3, 3, -4, 5, 3, 3};

TEST(Smooth, ClustersAndOutliers) {
  std::vector<double> values = bat_usage;
  values.push_back(unknown);
  const std::vector<double> smoothed = datasets::smooth(values, first);
  ASSERT_EQ(smoothed.size(), values.size());
  for (std::size_t i = 0; i < bat_usage.size(); ++i) {
    if (bat_usage[i] == -4) {
      EXPECT_TRUE(std::isnan(smoothed[i])) << i;
    } else if (bat_usage[i] < 4) {
      EXPECT_NEAR(smoothed[i], (3 * 10 + 3.25 + 2.97) / 12, 1e-12) << i;
    } else {
      EXPECT_NEAR(smoothed[i], (5 * 4 + 5.05) / 5, 1e-12) << i;
    }
  }
  EXPECT_TRUE(std::isnan(smoothed.back()));
}

// σ / μ no depende de las unidades: lo mismo por mil.
TEST(Smooth, TheScaleDoesNotMatter) {
  const std::vector<double>& small = bat_usage;
  std::vector<double> large;
  for (const double value : small) {
    large.push_back(value * 1000);
  }
  const std::vector<double> a = datasets::smooth(small, first);
  const std::vector<double> b = datasets::smooth(large, first);
  for (std::size_t i = 0; i < a.size(); ++i) {
    if (std::isnan(a[i])) {
      EXPECT_TRUE(std::isnan(b[i])) << i;
    } else {
      EXPECT_NEAR(a[i] * 1000, b[i], 1e-6) << i;
    }
  }
}

// Sin dos valores distintos, nada que agrupar.
TEST(Smooth, AConstantStays) {
  const std::vector<double> values{4, unknown, 4};
  const std::vector<double> smoothed = datasets::smooth(values, unused);
  EXPECT_EQ(smoothed[0], 4.0);
  EXPECT_TRUE(std::isnan(smoothed[1]));
  EXPECT_EQ(smoothed[2], 4.0);
}

}  // namespace
