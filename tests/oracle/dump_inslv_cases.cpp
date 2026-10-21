// Genera los casos de equivalencia de INSLV (vendor/inslv/tests/cases) con el PlanMiner
// original.
//
//   dump_inslv_cases <directorio de salida> <prefijo>=<traza.pts>...
//
// Para cada traza repite el pipeline de Old/src/bin/mainPlanMiner.cpp hasta INSLV: parser,
// tablas, discretización e inslv(). Por cada acción y conjunto de ejemplos (lógico y numérico)
// escribe <prefijo>-<acción>-<logic|numeric>.case con la entrada exacta de inslv() y las reglas
// que devuelve. inslv() escribe sus informes en el directorio de trabajo (res/ y traza.txt).

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "DataSLib.hpp"
#include "PtLib.hpp"
#include "RuleLib.hpp"

namespace {

using Table = std::vector<std::vector<double>>;
using Labels = std::vector<std::pair<std::string, std::string>>;
using LegacyRules = std::vector<std::pair<int, std::list<std::pair<std::string, std::string>>>>;

constexpr double legacy_missing = -999999999.0;

std::string format(double value) {
  if (value == legacy_missing) {
    return "?";
  }
  std::ostringstream out;
  out.precision(17);
  out << value;
  return out.str();
}

void write_variable(std::ostream& out, const std::string& kind, const std::string& name,
                    const std::vector<FuzzSet>& sets) {
  out << kind << ' ' << sets.size() << ' ' << name << '\n';
  for (const FuzzSet& set : sets) {
    out << "label " << format(set.a) << ' ' << format(set.b) << ' ' << format(set.c) << ' '
        << format(set.d) << ' ' << set.label << '\n';
  }
}

// La clase de cada ejemplo, como índice de la etiqueta de Class cuyo centro es su valor.
std::size_t class_index(double value, const std::vector<FuzzSet>& classes) {
  for (std::size_t i = 0; i < classes.size(); ++i) {
    if ((classes[i].b + classes[i].c) / 2.0 == value) {
      return i;
    }
  }
  std::cerr << "error: valor de clase sin etiqueta: " << value << '\n';
  std::exit(EXIT_FAILURE);
}

void write_case(const std::filesystem::path& file, const std::string& source, const Table& table,
                const Labels& labels, std::map<std::string, std::vector<FuzzSet>>& sets,
                const LegacyRules& rules) {
  std::ofstream out(file);
  out << "# Caso de equivalencia de INSLV: " << source << '\n';
  out << "# Generado con Old/ por tests/oracle/dump_inslv_cases.cpp: no editar a mano.\n";

  const std::size_t attributes = labels.size() - 1;  // la última columna es Class
  out << "attributes " << attributes << '\n';
  for (std::size_t i = 0; i < attributes; ++i) {
    write_variable(out, "attribute", labels[i].first, sets[labels[i].first]);
  }
  const std::vector<FuzzSet>& classes = sets[labels.back().first];
  write_variable(out, "target", labels.back().first, classes);

  const std::size_t examples = table.front().size();
  out << "examples " << examples << '\n';
  for (std::size_t e = 0; e < examples; ++e) {
    for (std::size_t i = 0; i < attributes; ++i) {
      out << format(table[i][e]) << ' ';
    }
    out << class_index(table.back()[e], classes) << '\n';
  }

  out << "rules " << rules.size() << '\n';
  for (const auto& [covered, conditions] : rules) {
    out << "rule " << covered << ' ' << conditions.size() << '\n';
    for (const auto& [attribute, value] : conditions) {
      out << "condition " << value << ' ' << attribute << '\n';
    }
  }
}

void dump(const std::string& prefix, const std::string& trace,
          const std::filesystem::path& output) {
  std::vector<PlanTrace*>* plans = parse(trace.c_str());
  for (PlanTrace* plan : *plans) {
    plan->schematize();
  }
  std::map<std::string, StatesLists> grouped = groupTaskStates(plans);

  std::map<std::string, std::vector<Table>> tables;
  std::map<std::string, std::vector<Labels>> labels;
  for (auto& [task, states] : grouped) {
    std::vector<Table> task_tables;
    std::vector<Labels> task_labels;
    to_table(task, states, &task_tables, &task_labels);
    tables[task] = task_tables;
    labels[task] = task_labels;
  }

  const std::vector<std::string> kinds{"logic", "numeric"};
  for (auto& [task, task_tables] : tables) {
    for (std::size_t kind = 0; kind < kinds.size(); ++kind) {
      if (labels[task][kind].size() <= 1) {
        continue;
      }
      Table& table = task_tables[kind];
      std::map<std::string, std::vector<FuzzSet>> sets = discretize(&table, &labels[task][kind]);
      LegacyRules rules;
      inslv(prefix, task, "./", &table, labels[task][kind], sets, &rules);

      const std::string name = prefix + "-" + task + "-" + kinds[kind];
      const std::string source = std::filesystem::path(trace).filename().string() + ", acción " +
                                 task + ", conjunto " + kinds[kind];
      write_case(output / (name + ".case"), source, table, labels[task][kind], sets, rules);
      std::cout << name << ": " << rules.size() << " reglas\n";
    }
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "uso: dump_inslv_cases <directorio de salida> <prefijo>=<traza.pts>...\n";
    return 2;
  }
  const std::filesystem::path output = std::filesystem::absolute(argv[1]);
  std::filesystem::create_directories(output);
  std::filesystem::create_directories("res");

  for (int i = 2; i < argc; ++i) {
    const std::string spec{argv[i]};
    const std::size_t equals = spec.find('=');
    if (equals == std::string::npos) {
      std::cerr << "error: se esperaba <prefijo>=<traza.pts>: " << spec << '\n';
      return 2;
    }
    dump(spec.substr(0, equals), spec.substr(equals + 1), output);
  }
  return 0;
}
