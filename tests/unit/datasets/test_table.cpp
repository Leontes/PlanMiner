#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

#include "datasets/table.hpp"

namespace {

namespace fs = std::filesystem;

using datasets::AttributeKind;

// pick-up a, con (on a b) desconocido en el estado de después y un estado que falta al final.
std::vector<plantraces::Trace> blocks() {
  return {plantraces::parse_trace(R"json({
    "format": "pddldomains-trace", "version": 1, "domain": {"name": "blocks"},
    "problem": {"name": "p"}, "world": "closed", "objects": {"a": "object", "b": "object"},
    "states": [
      {"step": 0, "time": 0, "true": ["(clear a)", "(handempty)", "(on a b)"], "unknown": [],
       "values": {}, "unknown_values": []},
      {"step": 1, "time": 1, "true": ["(clear b)", "(holding a)"], "unknown": ["(on a b)"],
       "values": {}, "unknown_values": []},
      {"step": 2, "time": 2, "missing": true}
    ],
    "actions": [
      {"step": 1, "time": 1, "name": "unstack", "arguments": ["a", "b"]},
      {"step": 2, "time": 2, "name": "put-down", "arguments": ["a"]}
    ]
  })json")};
}

std::vector<std::string> names(const datasets::Table& table) {
  std::vector<std::string> result;
  for (const datasets::Attribute& attribute : table.attributes) {
    result.push_back(datasets::to_string(attribute));
  }
  return result;
}

// El valor de la columna en la fila, o -1 si es desconocido (para comparar con EXPECT_EQ).
double at(const datasets::Table& table, const std::string& attribute, std::size_t row) {
  const std::vector<std::string> all = names(table);
  const auto found = std::find(all.begin(), all.end(), attribute);
  if (found == all.end()) {
    ADD_FAILURE() << "no hay columna " << attribute;
    return -2;
  }
  const double value = table.columns[static_cast<std::size_t>(found - all.begin())][row];
  return std::isnan(value) ? -1 : value;
}

TEST(BuildTable, OneRowPerStateAndOneColumnPerKnownAtom) {
  const std::vector<plantraces::Trace> traces = blocks();
  const auto actions = plantraces::group_by_action(traces);
  const datasets::Table table =
      datasets::build_table(plantraces::vocabulary(traces), actions.at("unstack"));

  EXPECT_EQ(table.rows(), 2U);
  EXPECT_FALSE(table.after(0));
  EXPECT_TRUE(table.after(1));
  // Las columnas, en el orden en que aparecen: los átomos del estado de antes (los falsos
  // también: en mundo cerrado, lo que no aparece es falso), el instante y lo nuevo de después.
  EXPECT_EQ(names(table),
            (std::vector<std::string>{"(clear ?0)", "(clear ?1)", "(handempty)", "(holding ?0)",
                                      "(holding ?1)", "(on ?0 ?0)", "(on ?0 ?1)", "(on ?1 ?0)",
                                      "(on ?1 ?1)", "time"}));
  EXPECT_EQ(at(table, "(clear ?0)", 0), 1);
  EXPECT_EQ(at(table, "(clear ?0)", 1), 0);
  EXPECT_EQ(at(table, "(holding ?0)", 1), 1);
  EXPECT_EQ(at(table, "(on ?0 ?1)", 0), 1);
  EXPECT_EQ(at(table, "(on ?0 ?1)", 1), -1);  // desconocido
  EXPECT_EQ(at(table, "time", 1), 1);
}

TEST(BuildTable, AMissingStateOnlyKnowsItsTime) {
  const std::vector<plantraces::Trace> traces = blocks();
  const auto actions = plantraces::group_by_action(traces);
  const datasets::Table table =
      datasets::build_table(plantraces::vocabulary(traces), actions.at("put-down"));

  ASSERT_EQ(table.rows(), 2U);
  for (std::size_t a = 0; a < table.attributes.size(); ++a) {
    if (table.attributes[a].kind == AttributeKind::time) {
      EXPECT_EQ(table.columns[a][1], 2.0);
    } else {
      EXPECT_TRUE(std::isnan(table.columns[a][1])) << datasets::to_string(table.attributes[a]);
    }
  }
}

TEST(BuildTable, ColumnsAreOnlyCreatedForKnownValues) {
  std::vector<plantraces::Trace> traces = blocks();
  // (on a b) desconocido también antes: (on ?0 ?1) no se conoce nunca y no es columna.
  traces[0].states[0].true_atoms.erase({"on", {"a", "b"}});
  traces[0].states[0].unknown_atoms.insert({"on", {"a", "b"}});
  const auto actions = plantraces::group_by_action(traces);
  const datasets::Table table =
      datasets::build_table(plantraces::vocabulary(traces), actions.at("unstack"));

  const std::vector<std::string> all = names(table);
  EXPECT_EQ(std::count(all.begin(), all.end(), "(on ?0 ?1)"), 0);
}

// Las trazas de tests/data/traces de una variante, de todos los problemas de la colección.
std::vector<plantraces::Trace> read_variant(const std::string& collection,
                                            const std::string& variant) {
  std::vector<fs::path> files;
  for (const auto& entry : fs::directory_iterator(fs::path{PLANMINER_TRACES_DIR} / collection)) {
    if (entry.path().filename().string().ends_with("." + variant + ".json")) {
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

class TableDataset : public testing::TestWithParam<std::string> {};

// Con las mismas observaciones, mundo cerrado y mundo abierto dan la misma tabla: que se listen
// o no los falsos no cambia lo que se sabe.
TEST_P(TableDataset, ClosedAndOpenWorldGiveTheSameTables) {
  const std::vector<plantraces::Trace> open = read_variant(GetParam(), "complete");
  const std::vector<plantraces::Trace> closed = read_variant(GetParam(), "closed");
  const auto open_actions = plantraces::group_by_action(open);
  const auto closed_actions = plantraces::group_by_action(closed);
  const plantraces::Vocabulary open_vocabulary = plantraces::vocabulary(open);
  const plantraces::Vocabulary closed_vocabulary = plantraces::vocabulary(closed);

  ASSERT_EQ(open_actions.size(), closed_actions.size());
  for (const auto& [name, transitions] : open_actions) {
    const datasets::Table a = datasets::build_table(open_vocabulary, transitions);
    const datasets::Table b = datasets::build_table(closed_vocabulary, closed_actions.at(name));
    EXPECT_EQ(a.attributes, b.attributes) << name;
    EXPECT_EQ(a.columns, b.columns) << name;  // las trazas completas no tienen desconocidos
  }
}

INSTANTIATE_TEST_SUITE_P(Collections, TableDataset,
                         testing::Values("blocks-strips-untyped", "zenotravel"),
                         [](const testing::TestParamInfo<std::string>& collection) {
                           std::string name = collection.param;
                           std::replace(name.begin(), name.end(), '-', '_');
                           return name;
                         });

}  // namespace
