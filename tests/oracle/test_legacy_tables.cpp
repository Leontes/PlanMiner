// Oráculo de las tablas de ejemplos: datasets::build_table frente al PlanMiner original con las
// mismas trazas, convertidas al formato antiguo (pts.hpp).
//
// Del original se toma la tabla que construye to_table antes de separar los atributos lógicos de
// los numéricos: parse, schematize, groupTaskStates y State::to_table de cada estado. Tienen que
// coincidir las columnas (nombre y orden) y los valores.
//
// Se usan las trazas de mundo abierto sin estados que faltan: con estados que faltan, el
// original desempareja los estados de antes y de después, y la tabla nueva los mantiene
// emparejados.

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "PtLib.hpp"
#include "datasets/table.hpp"
#include "plantraces/actions.hpp"
#include "pts.hpp"

namespace {

namespace fs = std::filesystem;

constexpr double legacy_missing = -999999999.0;

struct Case {
  std::string collection;
  std::string variant;
};

std::vector<plantraces::Trace> read_traces(const Case& c) {
  std::vector<fs::path> files;
  for (const auto& entry : fs::directory_iterator(fs::path{PLANMINER_TRACES_DIR} / c.collection)) {
    if (entry.path().filename().string().ends_with("." + c.variant + ".json")) {
      files.push_back(entry.path());
    }
  }
  std::sort(files.begin(), files.end());
  std::vector<plantraces::Trace> traces;
  for (const fs::path& file : files) {
    traces.push_back(plantraces::read_trace(file));
  }
  return traces;
}

struct LegacyTable {
  std::vector<std::vector<double>> columns;
  std::vector<std::pair<std::string, std::string>> labels;
};

// La primera parte de to_table en Old/src/PlanTraces/Utils.cpp: la tabla de todos los atributos.
LegacyTable legacy_table(StatesLists& states) {
  LegacyTable table;
  const auto rows = static_cast<unsigned int>(states.first.size() + states.second.size());
  unsigned int index = 0;
  for (State* state : states.first) {
    state->to_table(&table.columns, &table.labels, rows, &index);
  }
  for (State* state : states.second) {
    state->to_table(&table.columns, &table.labels, rows, &index);
  }
  return table;
}

// El nombre que da el original al atributo: ( on ?param_0 ~ tipo ?param_1 ~ tipo ). Los tipos
// son los de los argumentos de la primera aplicación de la acción.
std::string legacy_label(const datasets::Attribute& attribute,
                         const plantraces::Transition& transition) {
  if (attribute.kind == datasets::AttributeKind::time) {
    return "TIMESTAMP";
  }
  std::string label = "( " + attribute.term.name + " ";
  for (const std::size_t parameter : attribute.term.parameters) {
    const std::string& object = transition.action->arguments[parameter];
    label += "?param_" + std::to_string(parameter) + " ~ " +
             transition.trace->objects.at(object).front() + " ";
  }
  return label + ")";
}

class LegacyTables : public testing::TestWithParam<Case> {};

TEST_P(LegacyTables, SameTablesAsTheOriginal) {
  const std::vector<plantraces::Trace> traces = read_traces(GetParam());
  ASSERT_FALSE(traces.empty());

  const fs::path work = fs::path{PLANMINER_ORACLE_WORK_DIR} /
                        (GetParam().collection + "." + GetParam().variant + ".pts");
  fs::create_directories(work.parent_path());
  std::ofstream(work) << oracle::to_pts(traces);

  // El original no libera las trazas que devuelve.
  std::vector<PlanTrace*>* plans = parse(work.c_str());
  ASSERT_EQ(plans->size(), traces.size());
  for (PlanTrace* plan : *plans) {
    plan->schematize();
  }
  std::map<std::string, StatesLists> legacy_actions = groupTaskStates(plans);

  const plantraces::Vocabulary vocabulary = plantraces::vocabulary(traces);
  const auto actions = plantraces::group_by_action(traces);
  ASSERT_EQ(legacy_actions.size(), actions.size());

  for (const auto& [name, transitions] : actions) {
    SCOPED_TRACE("acción " + name);
    ASSERT_TRUE(legacy_actions.contains(name));
    const LegacyTable expected = legacy_table(legacy_actions[name]);
    const datasets::Table table = datasets::build_table(vocabulary, transitions);

    ASSERT_EQ(table.attributes.size(), expected.labels.size());
    for (std::size_t a = 0; a < table.attributes.size(); ++a) {
      ASSERT_EQ(legacy_label(table.attributes[a], transitions.front()), expected.labels[a].first)
          << "columna " << a;
      ASSERT_EQ(table.columns[a].size(), expected.columns[a].size());
      for (std::size_t row = 0; row < table.rows(); ++row) {
        const double value = table.columns[a][row];
        const double legacy = expected.columns[a][row];
        if (std::isnan(value)) {
          EXPECT_EQ(legacy, legacy_missing) << expected.labels[a].first << ", fila " << row;
        } else {
          EXPECT_EQ(value, legacy) << expected.labels[a].first << ", fila " << row;
        }
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(Traces, LegacyTables,
                         testing::Values(Case{"blocks-strips-untyped", "complete"},
                                         Case{"blocks-strips-untyped", "missing30"},
                                         Case{"blocks-strips-untyped", "noise5"},
                                         Case{"zenotravel", "complete"},
                                         Case{"zenotravel", "missing30"},
                                         Case{"zenotravel", "noise5"}),
                         [](const testing::TestParamInfo<Case>& c) {
                           std::string name = c.param.collection + "_" + c.param.variant;
                           std::replace(name.begin(), name.end(), '-', '_');
                           return name;
                         });

}  // namespace
