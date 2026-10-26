// Oráculo de la jerarquía de tipos: plantraces::type_hierarchy frente a extractTypeHierarchy del
// PlanMiner original (Old/src/PlanTraces/Utils.cpp), con las mismas trazas en el formato antiguo.
// Tienen que coincidir los tipos (en orden, con su padre) y las cabeceras de las acciones y de los
// predicados y fluentes.
//
// Sin trazas con estados que faltan: el original los empareja mal.

#include <gtest/gtest.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "PtLib.hpp"
#include "legacy.hpp"
#include "plantraces/types.hpp"
#include "pts.hpp"

namespace {

namespace fs = std::filesystem;

using oracle::Case;
using Headers = std::map<std::string, std::vector<std::pair<std::string, std::string>>>;

// Las cabeceras con los nombres de parámetro del original: param_0, param_1...
Headers legacy_headers(const std::map<std::string, std::vector<std::string>>& headers) {
  Headers result;
  for (const auto& [name, types] : headers) {
    std::vector<std::pair<std::string, std::string>>& header = result[name];
    for (std::size_t p = 0; p < types.size(); ++p) {
      header.emplace_back("param_" + std::to_string(p), types[p]);
    }
  }
  return result;
}

class LegacyTypes : public testing::TestWithParam<Case> {};

TEST_P(LegacyTypes, SameHierarchyAsTheOriginal) {
  const std::vector<plantraces::Trace> traces = oracle::read_traces(GetParam());
  ASSERT_FALSE(traces.empty());
  const fs::path work =
      fs::path{PLANMINER_ORACLE_WORK_DIR} /
      (std::string{testing::UnitTest::GetInstance()->current_test_info()->name()} + ".pts");
  fs::create_directories(work.parent_path());
  std::ofstream(work) << oracle::to_pts(traces);

  // El original no libera las trazas ni los tipos que devuelve.
  std::vector<PlanTrace*>* plans = parse(work.c_str());
  for (PlanTrace* plan : *plans) {
    plan->schematize();
  }
  Headers actions;
  Headers predicates;
  std::vector<plantraces::Type> expected;
  for (const ::Type* type : extractTypeHierarchy(plans, &actions, &predicates)) {
    expected.push_back({type->name, type->parent != nullptr ? type->parent->name : ""});
  }

  const plantraces::TypeHierarchy hierarchy = plantraces::type_hierarchy(traces);
  EXPECT_EQ(hierarchy.types, expected);
  EXPECT_EQ(legacy_headers(hierarchy.actions), actions);
  EXPECT_EQ(legacy_headers(hierarchy.predicates), predicates);
}

INSTANTIATE_TEST_SUITE_P(Traces, LegacyTypes,
                         testing::Values(Case{"blocks-strips-untyped", "complete"},
                                         Case{"blocks-strips-untyped", "missing30"},
                                         Case{"blocks-strips-untyped", "noise5"},
                                         Case{"zenotravel", "complete"},
                                         Case{"zenotravel", "missing30"},
                                         Case{"zenotravel", "noise5"}, Case{"counters", "complete"},
                                         Case{"counters", "missing30"}, Case{"counters", "noise5"}),
                         [](const testing::TestParamInfo<Case>& c) {
                           return oracle::case_name(c.param);
                         });

}  // namespace
