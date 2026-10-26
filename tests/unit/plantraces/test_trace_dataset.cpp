// Las trazas de tests/data/traces (scripts/generate_traces.py): se leen sin errores y cuadran con
// sus planes y entre sí.

#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "plantraces/trace.hpp"

namespace {

namespace fs = std::filesystem;

using plantraces::Trace;

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
  fs::path path(const std::string& name) const {
    const fs::path& plan = GetParam();
    return plan.parent_path() / (plan.stem().string() + "." + name + ".json");
  }
  Trace variant(const std::string& name) const { return plantraces::read_trace(path(name)); }
  nlohmann::json json(const std::string& name) const {
    return nlohmann::json::parse(std::ifstream(path(name)));
  }
};

TEST_P(TraceDataset, EveryVariantFollowsThePlan) {
  const std::vector<std::string> expected = plan_actions(GetParam());
  ASSERT_FALSE(expected.empty());

  for (const std::string name : {"complete", "closed", "missing30", "noise5", "missing-states20"}) {
    const Trace trace = variant(name);
    ASSERT_EQ(trace.states.size(), expected.size() + 1) << name;
    for (std::size_t i = 0; i < expected.size(); ++i) {
      const plantraces::GroundAction& action = trace.actions[i];
      EXPECT_EQ(plantraces::to_string({action.name, action.arguments}), expected[i])
          << name << ", acción " << i + 1;
    }
  }
}

TEST_P(TraceDataset, CompleteTracesHaveNoGaps) {
  for (const std::string name : {"complete", "closed"}) {
    for (const plantraces::Observation& state : variant(name).states) {
      EXPECT_FALSE(state.missing) << name << ", estado " << state.step;
      EXPECT_TRUE(state.unknown_atoms.empty()) << name << ", estado " << state.step;
      EXPECT_TRUE(state.unknown_values.empty()) << name << ", estado " << state.step;
    }
  }
}

TEST_P(TraceDataset, ClosedAndOpenWorldAgree) {
  const Trace open = variant("complete");
  const Trace closed = variant("closed");
  EXPECT_EQ(open.world, plantraces::World::open);
  EXPECT_EQ(closed.world, plantraces::World::closed);

  ASSERT_EQ(open.states.size(), closed.states.size());
  for (std::size_t i = 0; i < open.states.size(); ++i) {
    EXPECT_EQ(open.states[i].true_atoms, closed.states[i].true_atoms) << "estado " << i;
    EXPECT_EQ(open.states[i].values, closed.states[i].values) << "estado " << i;
    EXPECT_TRUE(closed.states[i].false_atoms.empty()) << "estado " << i;
    for (const plantraces::GroundAtom& atom : open.states[i].false_atoms) {
      EXPECT_EQ(closed.states[i].truth(atom), false)
          << "estado " << i << ", " << plantraces::to_string(atom);
    }
  }
}

TEST_P(TraceDataset, DegradedTracesHaveGaps) {
  // Los huecos están en los átomos o en los valores (counters solo tiene fluentes).
  for (const plantraces::Observation& state : variant("missing30").states) {
    EXPECT_FALSE(state.unknown_atoms.empty() && state.unknown_values.empty())
        << "missing30, estado " << state.step;
  }

  const Trace missing_states = variant("missing-states20");
  EXPECT_TRUE(std::any_of(missing_states.states.begin(), missing_states.states.end(),
                          [](const plantraces::Observation& state) { return state.missing; }));

  // El ruido invierte átomos o altera valores: algún estado dice algo distinto que la traza
  // completa. Salvo si no ha invertido ni alterado nada: el porcentaje se redondea en cada estado,
  // y en uno con pocos valores (counters p01, con 5) el 5 % es 0. La traza lo anota.
  const Trace complete = variant("complete");
  const Trace noisy = variant("noise5");
  bool differs = false;
  for (std::size_t i = 0; i < complete.states.size(); ++i) {
    differs = differs || complete.states[i].true_atoms != noisy.states[i].true_atoms ||
              complete.states[i].values != noisy.states[i].values;
  }
  const nlohmann::json degradation = json("noise5").at("degradation");
  const auto altered = degradation.at("flipped_atoms").get<std::size_t>() +
                       degradation.at("perturbed_values").get<std::size_t>();
  EXPECT_EQ(differs, altered > 0) << altered << " átomos invertidos y valores alterados";
}

INSTANTIATE_TEST_SUITE_P(Problems, TraceDataset, testing::ValuesIn(plans()),
                         [](const testing::TestParamInfo<fs::path>& plan) {
                           std::string name = plan.param.parent_path().filename().string() + "_" +
                                              plan.param.stem().string();
                           std::replace(name.begin(), name.end(), '-', '_');
                           return name;
                         });

}  // namespace
