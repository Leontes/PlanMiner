// La discretización del original. Que dé los mismos números que el original lo comprueba el
// oráculo (tests/oracle/test_legacy_discretization.cpp); aquí, su comportamiento en casos pequeños.

#include <gtest/gtest.h>

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

// HRC: los grupos bien separados se quedan juntos; el de 1 y 10 se parte otra vez.
TEST(HierarchicalDiscretize, SeparatedGroups) {
  EXPECT_EQ(datasets::hierarchical_discretize({1, 1, 1, 10, 10, 10, 20, 20, 20, unknown}, unused),
            (std::vector<FuzzySet>{{1, 1, 1, 1}, {10, 10, 10, 10}, {20, 20, 20, 20}}));
}

// Un valor aislado queda solo en su cluster, que habría que partir: la rama lo descarta.
TEST(HierarchicalDiscretize, SingletonsAreDroppedOrKept) {
  const std::vector<double> values{0, 0, 0, 0, 5, 10, 10, 10, 10};
  EXPECT_EQ(datasets::hierarchical_discretize(values, first),
            (std::vector<FuzzySet>{{0, 0, 0, 0}, {10, 10, 10, 10}}));
  datasets::HierarchicalOptions options;
  options.keep_singletons = true;
  EXPECT_EQ(datasets::hierarchical_discretize(values, first, options),
            (std::vector<FuzzySet>{{0, 0, 0, 0}, {5, 5, 5, 5}, {10, 10, 10, 10}}));
}

// Sin normalizar, σ / n depende de las unidades: con valores de 100 en 100, los grupos se parten
// hasta quedarse en valores sueltos, y se descartan todos. Normalizados, quedan dos.
TEST(HierarchicalDiscretize, NormalizedValues) {
  const std::vector<double> values{100, 101, 102, 200, 201, 202};
  EXPECT_TRUE(datasets::hierarchical_discretize(values, unused).empty());
  datasets::HierarchicalOptions options;
  options.normalize = true;
  EXPECT_EQ(datasets::hierarchical_discretize(values, unused, options),
            (std::vector<FuzzySet>{{100, 100, 102, 102}, {200, 200, 202, 202}}));
}

TEST(HierarchicalDiscretize, AConstantOrNothing) {
  EXPECT_EQ(datasets::hierarchical_discretize({4, unknown, 4}, unused),
            (std::vector<FuzzySet>{{4, 4, 4, 4}}));
  EXPECT_TRUE(datasets::hierarchical_discretize({unknown}, unused).empty());
}

}  // namespace
