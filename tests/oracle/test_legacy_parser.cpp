// Oráculo del parser de trazas. Cuando se porte, estos tests compararán el parser nuevo con
// el original (parse() de Old/) sobre las mismas trazas.

#include <gtest/gtest.h>

#include <cstddef>
#include <string>

#include "PtLib.hpp"

namespace {

struct TraceFile {
  std::string file;
  std::size_t plans;
  std::size_t first_plan_length;
};

class LegacyParser : public testing::TestWithParam<TraceFile> {};

TEST_P(LegacyParser, ReadsEveryPlan) {
  const TraceFile& trace = GetParam();
  const std::string path = std::string{PLANMINER_LEGACY_TRACES} + "/" + trace.file;

  // El original no libera las trazas que devuelve.
  const std::vector<PlanTrace*>* plans = parse(path.c_str());

  ASSERT_EQ(plans->size(), trace.plans);
  EXPECT_EQ(plans->front()->getNTS(), trace.first_plan_length);
}

INSTANTIATE_TEST_SUITE_P(Traces, LegacyParser,
                         testing::Values(TraceFile{"BlocksWorld_STRIPS.pts", 32, 40},
                                         TraceFile{"Depots_Numerical.pts", 13, 19},
                                         TraceFile{"Zenotravel_Numerical1.pts", 24, 43}),
                         [](const testing::TestParamInfo<TraceFile>& trace) {
                           return trace.param.file.substr(0, trace.param.file.find('.'));
                         });

}  // namespace
