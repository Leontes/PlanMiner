// Oráculo de las tablas de ejemplos: datasets::build_table y datasets::numeric_relations frente al
// PlanMiner original con las mismas trazas, convertidas al formato antiguo (pts.hpp).
//
// Del original se toma la tabla que construye to_table antes de separar los atributos lógicos de
// los numéricos: parse, schematize, groupTaskStates y State::to_table de cada estado. Tienen que
// coincidir las columnas (nombre y orden) y los valores. Y lo mismo con las relaciones numéricas
// que añade to_table al final.
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
#include "datasets/relations.hpp"
#include "datasets/table.hpp"
#include "legacy.hpp"
#include "plantraces/actions.hpp"
#include "pts.hpp"

// Del original (Old/src/PlanTraces/Utils.cpp), que no las declara en sus cabeceras.
bool vectorInutil(std::vector<double> vec);
bool vectorDeZeros(std::vector<double> vec);
std::string getEquiv(std::string pred, std::vector<std::pair<std::string, std::string>> vectEquiv);
bool esPrecondicionable(std::string pred,
                        std::vector<std::pair<std::string, std::vector<double>>> listadeDiffs);

namespace {

namespace fs = std::filesystem;

using oracle::Case;
using oracle::legacy_label;
using oracle::legacy_missing;
using oracle::legacy_relation_label;
using oracle::read_traces;

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

// El final de to_table en Old/src/PlanTraces/Utils.cpp: las relaciones numéricas. Es el código
// original copiado, con los nombres de sus variables, porque no se puede llamar sin lanzar antes
// el A*, que con zenotravel tarda cientos de segundos. Los cambios: los tres bloques casi iguales
// de =, < y > van en un bucle, y lo que depende del A* (el nombre de las relaciones con el cambio
// de un fluente, que en el original lleva la expresión encontrada) es aquí "DELTA <fluente>". El
// original compara con el cambio con el signo cambiado: con lo que baja el fluente.
LegacyTable legacy_relations(const LegacyTable& all, unsigned int preStates) {
  const auto nStates = static_cast<unsigned int>(all.columns.front().size());

  // Dataset con los predicados numericos
  std::vector<std::vector<double>> dataset;
  std::vector<std::pair<std::string, std::string>> attribLabels;
  for (unsigned int attr = 0; attr < all.labels.size(); attr++) {
    if (all.labels[attr].second == "Numerical") {
      dataset.push_back(all.columns[attr]);
      attribLabels.push_back(all.labels[attr]);
    }
  }

  // 1º Calcular los diffs
  std::vector<double> nAttrib;
  bool tieneAlgo = false;
  std::vector<std::pair<std::string, std::vector<double>>> listadeDiffs;
  for (unsigned int attr = 0; attr < attribLabels.size(); attr++) {
    nAttrib.clear();
    tieneAlgo = false;
    for (unsigned int i = 0; i < preStates; i++) {
      if (dataset[attr][i] != legacy_missing and dataset[attr][i + preStates] != legacy_missing) {
        nAttrib.push_back(dataset[attr][i + preStates] - dataset[attr][i]);
        tieneAlgo = true;
      } else {
        nAttrib.push_back(legacy_missing);
      }
    }
    if (tieneAlgo == true and vectorDeZeros(nAttrib) == true) {
      listadeDiffs.emplace_back(attribLabels[attr].first, nAttrib);
    }
  }

  // Lo que pondría el A*.
  std::vector<std::pair<std::string, std::string>> vectEquiv;
  for (const auto& diff : listadeDiffs) {
    vectEquiv.emplace_back("DELTA - " + diff.first, "DELTA " + diff.first);
  }

  // Rellenamos los huecos que faltan en los vectores de diferencias
  std::vector<double> aux;
  for (auto& diff : listadeDiffs) {
    aux = diff.second;
    aux.insert(aux.end(), diff.second.begin(), diff.second.end());
    diff.second = aux;
  }

  LegacyTable out;
  const auto relation = [&](const std::string& op, unsigned int attr, const std::string& other,
                            const std::vector<double>& values) {
    if (tieneAlgo == true and vectorInutil(values) == true) {
      out.labels.emplace_back(op + " - " + attribLabels[attr].first + "  " + other, "Logical");
      out.columns.push_back(values);
    }
  };

  for (unsigned int attr = 0; attr < attribLabels.size(); attr++) {
    if (!esPrecondicionable(attribLabels[attr].first, listadeDiffs)) {
      continue;
    }
    for (unsigned int attr2 = attr; attr2 < attribLabels.size(); attr2++) {
      if (attr != attr2 and attribLabels[attr2].first != "TIMESTAMP") {
        for (const std::string op : {"opEQ", "opLT", "opGT"}) {
          nAttrib.clear();
          tieneAlgo = false;
          for (unsigned int i = 0; i < nStates; i++) {
            if (dataset[attr][i] != legacy_missing and dataset[attr2][i] != legacy_missing) {
              const double a = dataset[attr][i];
              const double b = dataset[attr2][i];
              nAttrib.push_back((op == "opEQ" ? a == b : op == "opLT" ? a < b : a > b) ? 1.0 : 0.0);
              tieneAlgo = true;
            } else {
              nAttrib.push_back(legacy_missing);
            }
          }
          relation(op, attr, attribLabels[attr2].first, nAttrib);
        }
      }
    }

    // Repetimos para los diffs
    for (unsigned int attr2 = 0; attr2 < listadeDiffs.size(); attr2++) {
      if (attribLabels[attr].first != listadeDiffs[attr2].first and
          listadeDiffs[attr2].first != "TIMESTAMP") {
        const std::vector<double>& diff = listadeDiffs[attr2].second;
        const unsigned int half = static_cast<unsigned int>(diff.size() / 2);
        for (const std::string op : {"opEQ", "opLT", "opGT"}) {
          nAttrib.clear();
          tieneAlgo = false;
          // El for hay que partirlo en dos trozos para que asigne correctamente la info a los
          // pre y post estados
          for (unsigned int i = 0; i < half; i++) {
            if (dataset[attr][i] != legacy_missing and diff[i + half] != legacy_missing) {
              const double a = dataset[attr][i];
              const double b = -(diff[i + half]);
              nAttrib.push_back((op == "opEQ" ? a == b : op == "opLT" ? a < b : a > b) ? 1.0 : 0.0);
              tieneAlgo = true;
            } else {
              nAttrib.push_back(legacy_missing);
            }
          }
          for (unsigned int i = 0; i < half; i++) {
            if (dataset[attr][i + half] != legacy_missing and diff[i] != legacy_missing) {
              const double a = dataset[attr][i + half];
              const double b = -(diff[i]);
              nAttrib.push_back((op == "opEQ" ? a == b : op == "opLT" ? a < b : a > b) ? 1.0 : 0.0);
              tieneAlgo = true;
            } else {
              nAttrib.push_back(legacy_missing);
            }
          }
          relation(op, attr, getEquiv(listadeDiffs[attr2].first, vectEquiv), nAttrib);
        }
      }
    }
  }
  return out;
}

// Que las columnas nuevas, con el nombre que les da el original, sean las del original.
void expect_same_columns(const std::vector<std::string>& labels,
                         const std::vector<std::vector<double>>& columns,
                         const LegacyTable& expected) {
  ASSERT_EQ(labels.size(), expected.labels.size());
  for (std::size_t a = 0; a < labels.size(); ++a) {
    ASSERT_EQ(labels[a], expected.labels[a].first) << "columna " << a;
    ASSERT_EQ(columns[a].size(), expected.columns[a].size());
    for (std::size_t row = 0; row < columns[a].size(); ++row) {
      const double value = columns[a][row];
      const double legacy = expected.columns[a][row];
      if (std::isnan(value)) {
        EXPECT_EQ(legacy, legacy_missing) << labels[a] << ", fila " << row;
      } else {
        EXPECT_EQ(value, legacy) << labels[a] << ", fila " << row;
      }
    }
  }
}

// Las acciones de las trazas del caso, con las del original.
class LegacyTables : public testing::TestWithParam<Case> {
 protected:
  void SetUp() override {
    traces_ = read_traces(GetParam());
    ASSERT_FALSE(traces_.empty());

    // Uno por test: los de un mismo caso pueden ir en paralelo.
    const fs::path work =
        fs::path{PLANMINER_ORACLE_WORK_DIR} /
        (std::string{testing::UnitTest::GetInstance()->current_test_info()->name()} + ".pts");
    fs::create_directories(work.parent_path());
    std::ofstream(work) << oracle::to_pts(traces_);

    // El original no libera las trazas que devuelve.
    std::vector<PlanTrace*>* plans = parse(work.c_str());
    ASSERT_EQ(plans->size(), traces_.size());
    for (PlanTrace* plan : *plans) {
      plan->schematize();
    }
    legacy_actions_ = groupTaskStates(plans);

    vocabulary_ = plantraces::vocabulary(traces_);
    actions_ = plantraces::group_by_action(traces_);
    ASSERT_EQ(legacy_actions_.size(), actions_.size());
    for (const auto& [name, transitions] : actions_) {
      ASSERT_TRUE(legacy_actions_.contains(name)) << name;
    }
  }

  std::vector<plantraces::Trace> traces_;
  plantraces::Vocabulary vocabulary_;
  std::map<std::string, std::vector<plantraces::Transition>> actions_;
  std::map<std::string, StatesLists> legacy_actions_;
};

TEST_P(LegacyTables, SameTablesAsTheOriginal) {
  for (const auto& [name, transitions] : actions_) {
    SCOPED_TRACE("acción " + name);
    const datasets::Table table = datasets::build_table(vocabulary_, transitions);
    std::vector<std::string> labels;
    for (const datasets::Attribute& attribute : table.attributes) {
      labels.push_back(legacy_label(attribute, transitions.front()));
    }
    expect_same_columns(labels, table.columns, legacy_table(legacy_actions_[name]));
  }
}

TEST_P(LegacyTables, SameRelationsAsTheOriginal) {
  for (const auto& [name, transitions] : actions_) {
    SCOPED_TRACE("acción " + name);
    const datasets::Table table = datasets::build_table(vocabulary_, transitions);
    std::vector<std::string> labels;
    std::vector<std::vector<double>> columns;
    for (const datasets::Relation& relation :
         datasets::numeric_relations(table, {}, datasets::RelationOptions::original())) {
      labels.push_back(legacy_relation_label(relation, transitions.front()));
      columns.push_back(relation.column);
    }

    StatesLists& states = legacy_actions_[name];
    expect_same_columns(
        labels, columns,
        legacy_relations(legacy_table(states), static_cast<unsigned int>(states.first.size())));
  }
}

INSTANTIATE_TEST_SUITE_P(Traces, LegacyTables,
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
