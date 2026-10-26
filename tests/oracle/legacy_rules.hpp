// El pipeline del PlanMiner original hasta INSLV, en su propia unidad de compilación: las cabeceras
// del original declaran una función inslv() que choca con el espacio de nombres de vendor/inslv.
#pragma once

#include <array>
#include <filesystem>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace oracle {

/// Una etiqueta difusa de un atributo de los ejemplos.
struct LegacyLabel {
  std::string label;
  double a = 0.0;
  double b = 0.0;
  double c = 0.0;
  double d = 0.0;
};

using RuleList = std::vector<std::pair<int, std::list<std::pair<std::string, std::string>>>>;

/// Un conjunto de ejemplos de INSLV y sus reglas, con los nombres del original. Las reglas, como
/// las daba el original: los ejemplos cubiertos, en negativo si describen el estado de antes, y los
/// pares (atributo, valor).
struct LegacyExamples {
  std::vector<std::string> names;
  std::vector<std::vector<LegacyLabel>> labels;
  std::vector<std::vector<double>> values;
  std::vector<double> classes;
  RuleList rules;
};

/// La precondición y el efecto de una acción, como pares (atributo, valor).
struct LegacyFused {
  std::vector<std::pair<std::string, std::string>> precondition;
  std::vector<std::pair<std::string, std::string>> effect;
};

/// La fusión del original (fuseRules) de las reglas de una acción, con su cabecera: los pares
/// (parámetro, tipo).
LegacyFused legacy_fuse(RuleList rules, std::vector<std::pair<std::string, std::string>> header);

/// Los dos conjuntos de ejemplos de cada acción (lógico y numérico) y sus reglas, con el pipeline
/// del original (Old/src/bin/mainPlanMiner.cpp): las tablas de todas las acciones y, después, por
/// acción, discretize e inslv() de cada conjunto. Sin atributos, el original no aprende: nullopt.
/// Usa el rand() global tal como esté; inslv() escribe sus informes en res/.
std::map<std::string, std::array<std::optional<LegacyExamples>, 2>> legacy_rules(
    const std::filesystem::path& trace);

}  // namespace oracle
