// Oráculo de principio a fin: el dominio que aprende la configuración original nueva
// (pddl::learn_original y pddl::write_original) frente al que escribe el PlanMiner original
// (PlanMinerLegacy) con las mismas trazas.
//
// El original recibe las trazas en mayúsculas, como las suyas: su escritura del dominio busca las
// cabeceras de los predicados con el nombre en mayúsculas. Los dos dominios se comparan en
// minúsculas (el original deja el nombre de las acciones como viene).
//
// Solo con trazas en las que el A* del original es inmediato: blocksworld y counters sin ruido
// (ver test_legacy_rules.cpp).

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "legacy.hpp"
#include "pddl/domain.hpp"
#include "pts.hpp"

namespace {

namespace fs = std::filesystem;

using oracle::Case;

std::string lower(std::string text) {
  std::transform(text.begin(), text.end(), text.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return text;
}

class LegacyDomain : public testing::TestWithParam<Case> {};

TEST_P(LegacyDomain, SameDomainAsTheOriginal) {
  const std::vector<plantraces::Trace> traces = oracle::read_traces(GetParam());
  ASSERT_FALSE(traces.empty());

  // El original escribe en su directorio de trabajo: outputPM/, traza.txt y <nombre>.pddl.
  const fs::path work =
      fs::path{PLANMINER_ORACLE_WORK_DIR} / "domains" / oracle::case_name(GetParam());
  fs::remove_all(work);
  fs::create_directories(work);
  std::ofstream(work / "trace.pts") << oracle::to_pts(traces, true);
  const std::string command = "cd \"" + work.string() + "\" && \"" PLANMINER_LIMIT_MEMORY "\" " +
                              std::to_string(PLANMINER_LEGACY_MEMORY_MB) +
                              " \"" PLANMINER_LEGACY
                              "\" trace.pts oracle > stdout.log 2> stderr.log";
  ASSERT_EQ(std::system(command.c_str()), 0) << "salidas en " << work;
  std::ostringstream expected;
  expected << std::ifstream(work / "oracle.pddl").rdbuf();

  std::srand(1);
  const std::string learned = pddl::write_original("oracle", pddl::learn_original(traces));
  std::ofstream(work / "new.pddl") << learned;
  EXPECT_EQ(lower(learned), lower(expected.str())) << "dominios en " << work;
}

INSTANTIATE_TEST_SUITE_P(Traces, LegacyDomain,
                         testing::Values(Case{"blocks-strips-untyped", "complete"},
                                         Case{"blocks-strips-untyped", "missing30"},
                                         Case{"blocks-strips-untyped", "noise5"},
                                         Case{"counters", "complete"},
                                         Case{"counters", "missing30"}),
                         [](const testing::TestParamInfo<Case>& c) {
                           return oracle::case_name(c.param);
                         });

}  // namespace
