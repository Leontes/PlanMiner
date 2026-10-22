// Las trazas de tests/data/traces (scripts/generate_traces.py): se leen sin errores y cuadran con
// sus planes y entre sí.

#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "planminer/trace.hpp"

namespace {

namespace fs = std::filesystem;

using planminer::Trace;

std::vector<fs::path> plans() {
  std::vector<fs::path> files;
  for (const auto& entry : fs::recursive_directory_iterator(PLANMINER_TRACES_DIR)) {
    if (entry.path().extension() == ".plan") {
      files.push_back(entry.path());
    }
  }
  std::sort(files.begin(), files.end());
  return files;
}

// Las acciones del plan, una por línea: "(pick-up b)". Los comentarios empiezan por ';'.
std::vector<std::string> plan_actions(const fs::path& plan) {
  std::ifstream in(plan);
  std::vector<std::string> actions;
  for (std::string line; std::getline(in, line);) {
    if (line.starts_with("(")) {
      actions.push_back(line);
    }
  }
  return actions;
}

class TraceDataset : public testing::TestWithParam<fs::path> {
 protected:
  Trace variant(const std::string& name) const {
    const fs::path& plan = GetParam();
    return planminer::read_trace(plan.parent_path() /
                                 (plan.stem().string() + "." + name + ".json"));
  }
};

TEST_P(TraceDataset, EveryVariantFollowsThePlan) {
  const std::vector<std::string> expected = plan_actions(GetParam());
  ASSERT_FALSE(expected.empty());

  for (const std::string name : {"complete", "closed", "missing30", "noise5", "missing-states20"}) {
    const Trace trace = variant(name);
    ASSERT_EQ(trace.states.size(), expected.size() + 1) << name;
    for (std::size_t i = 0; i < expected.size(); ++i) {
      const planminer::GroundAction& action = trace.actions[i];
      EXPECT_EQ(planminer::to_string({action.name, action.arguments}), expected[i])
          << name << ", acción " << i + 1;
    }
  }
}

TEST_P(TraceDataset, CompleteTracesHaveNoGaps) {
  for (const std::string name : {"complete", "closed"}) {
    for (const planminer::Observation& state : variant(name).states) {
      EXPECT_FALSE(state.missing) << name << ", estado " << state.step;
      EXPECT_TRUE(state.unknown_atoms.empty()) << name << ", estado " << state.step;
      EXPECT_TRUE(state.unknown_values.empty()) << name << ", estado " << state.step;
    }
  }
}

TEST_P(TraceDataset, ClosedAndOpenWorldAgree) {
  const Trace open = variant("complete");
  const Trace closed = variant("closed");
  EXPECT_EQ(open.world, planminer::World::open);
  EXPECT_EQ(closed.world, planminer::World::closed);

  ASSERT_EQ(open.states.size(), closed.states.size());
  for (std::size_t i = 0; i < open.states.size(); ++i) {
    EXPECT_EQ(open.states[i].true_atoms, closed.states[i].true_atoms) << "estado " << i;
    EXPECT_EQ(open.states[i].values, closed.states[i].values) << "estado " << i;
    EXPECT_TRUE(closed.states[i].false_atoms.empty()) << "estado " << i;
    for (const planminer::GroundAtom& atom : open.states[i].false_atoms) {
      EXPECT_EQ(closed.states[i].truth(atom), false)
          << "estado " << i << ", " << planminer::to_string(atom);
    }
  }
}

TEST_P(TraceDataset, DegradedTracesHaveGaps) {
  for (const planminer::Observation& state : variant("missing30").states) {
    EXPECT_FALSE(state.unknown_atoms.empty()) << "missing30, estado " << state.step;
  }

  const Trace missing_states = variant("missing-states20");
  EXPECT_TRUE(std::any_of(missing_states.states.begin(), missing_states.states.end(),
                          [](const planminer::Observation& state) { return state.missing; }));

  // El ruido invierte átomos: algún estado dice algo distinto que la traza completa.
  const Trace complete = variant("complete");
  const Trace noisy = variant("noise5");
  bool differs = false;
  for (std::size_t i = 0; i < complete.states.size(); ++i) {
    differs = differs || complete.states[i].true_atoms != noisy.states[i].true_atoms;
  }
  EXPECT_TRUE(differs);
}

INSTANTIATE_TEST_SUITE_P(Problems, TraceDataset, testing::ValuesIn(plans()),
                         [](const testing::TestParamInfo<fs::path>& plan) {
                           std::string name = plan.param.parent_path().filename().string() + "_" +
                                              plan.param.stem().string();
                           std::replace(name.begin(), name.end(), '-', '_');
                           return name;
                         });

}  // namespace
