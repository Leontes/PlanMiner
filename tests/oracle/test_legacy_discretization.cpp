// Oráculo de la discretización: datasets::discretize frente a discretize del PlanMiner original
// (Old/src/Datasets/Utils.cpp), con las columnas numéricas (fluentes e instante) de las tablas de
// las trazas de prueba. Los dos resiembran los clusters vacíos con rand(), así que cada
// comparación empieza con la misma semilla.

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "DataSLib.hpp"
#include "datasets/discretization.hpp"
#include "datasets/table.hpp"
#include "legacy.hpp"
#include "plantraces/actions.hpp"
#include "plantraces/trace.hpp"

namespace {

using oracle::Case;
using oracle::legacy_missing;
using oracle::read_traces;

// La partición que da el original a una columna.
std::vector<datasets::FuzzySet> legacy_discretize(const std::vector<double>& column) {
  std::vector<std::vector<double>> dataset{column};
  for (double& value : dataset.front()) {
    if (std::isnan(value)) {
      value = legacy_missing;
    }
  }
  std::vector<std::pair<std::string, std::string>> labels{{"x", "Numerical"}};
  const std::map<std::string, std::vector<FuzzSet>> partitions = discretize(&dataset, &labels);
  std::vector<datasets::FuzzySet> sets;
  for (const FuzzSet& set : partitions.at("x")) {
    sets.push_back({set.a, set.b, set.c, set.d});
  }
  return sets;
}

class LegacyDiscretization : public testing::TestWithParam<Case> {};

TEST_P(LegacyDiscretization, SamePartitionsAsTheOriginal) {
  const std::vector<plantraces::Trace> traces = read_traces(GetParam());
  ASSERT_FALSE(traces.empty());
  const plantraces::Vocabulary vocabulary = plantraces::vocabulary(traces);

  std::size_t compared = 0;
  for (const auto& [name, transitions] : plantraces::group_by_action(traces)) {
    const datasets::Table table = datasets::build_table(vocabulary, transitions);
    for (std::size_t a = 0; a < table.attributes.size(); ++a) {
      if (table.attributes[a].kind == datasets::AttributeKind::atom) {
        continue;
      }
      SCOPED_TRACE(name + " " + datasets::to_string(table.attributes[a]));
      std::srand(1);
      const std::vector<datasets::FuzzySet> expected = legacy_discretize(table.columns[a]);
      std::srand(1);
      const std::vector<datasets::FuzzySet> sets =
          datasets::discretize(table.columns[a], [] { return std::rand(); });
      ASSERT_EQ(sets.size(), expected.size());
      for (std::size_t s = 0; s < sets.size(); ++s) {
        EXPECT_EQ(sets[s].a, expected[s].a) << "etiqueta " << s;
        EXPECT_EQ(sets[s].b, expected[s].b) << "etiqueta " << s;
        EXPECT_EQ(sets[s].c, expected[s].c) << "etiqueta " << s;
        EXPECT_EQ(sets[s].d, expected[s].d) << "etiqueta " << s;
      }
      ++compared;
    }
  }
  EXPECT_GT(compared, 0U);
}

INSTANTIATE_TEST_SUITE_P(Traces, LegacyDiscretization,
                         testing::Values(Case{"zenotravel", "complete"},
                                         Case{"zenotravel", "missing30"},
                                         Case{"zenotravel", "noise5"}, Case{"counters", "complete"},
                                         Case{"counters", "missing30"}, Case{"counters", "noise5"}),
                         [](const testing::TestParamInfo<Case>& c) {
                           return oracle::case_name(c.param);
                         });

}  // namespace
