// Lo que comparten los tests del oráculo: las trazas de prueba y los nombres que da el PlanMiner
// original a los atributos.
#pragma once

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

#include "datasets/relations.hpp"
#include "datasets/table.hpp"
#include "plantraces/actions.hpp"
#include "plantraces/trace.hpp"

namespace oracle {

/// El valor desconocido del original.
constexpr double legacy_missing = -999999999.0;

/// Una variante de una colección de las trazas de prueba.
struct Case {
  std::string collection;
  std::string variant;
};

/// "zenotravel_complete", para el nombre de los tests.
inline std::string case_name(const Case& c) {
  std::string name = c.collection + "_" + c.variant;
  std::replace(name.begin(), name.end(), '-', '_');
  return name;
}

inline std::vector<plantraces::Trace> read_traces(const Case& c) {
  namespace fs = std::filesystem;
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

/// El nombre que da el original a un átomo, un fluente o el instante:
/// "( on ?param_0 ~ tipo ?param_1 ~ tipo )" o "TIMESTAMP". Los tipos son los de los argumentos de
/// la primera aplicación de la acción.
inline std::string legacy_label(const datasets::Attribute& attribute,
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

/// El nombre que da el original a una relación: "opLT - ( fuel ... )  ( capacity ... )". En las
/// relaciones con lo que baja un fluente, el original pone la expresión de su cambio; aquí,
/// "DELTA <fluente>".
inline std::string legacy_relation_label(const datasets::Relation& relation,
                                         const plantraces::Transition& transition) {
  const char* op = "opEQ";
  if (relation.comparison == datasets::Comparison::less) {
    op = "opLT";
  } else if (relation.comparison == datasets::Comparison::greater) {
    op = "opGT";
  }
  const datasets::Attribute right{datasets::AttributeKind::fluent,
                                  relation.right.variables.front()};
  return std::string{op} + " - " + legacy_label(relation.left, transition) + "  " +
         (relation.right.kind == datasets::OperandKind::fall ? "DELTA " : "") +
         legacy_label(right, transition);
}

}  // namespace oracle
