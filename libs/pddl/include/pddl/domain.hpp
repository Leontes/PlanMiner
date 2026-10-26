// El dominio que aprende PlanMiner de las trazas y su escritura en PDDL.
#pragma once

#include <string>
#include <vector>

#include "plantraces/trace.hpp"
#include "plantraces/types.hpp"
#include "rules/examples.hpp"
#include "rules/fusion.hpp"

namespace pddl {

/// Lo que se aprende de una acción.
struct LearnedAction {
  std::string name;
  /// Los tipos de los argumentos de su primera aplicación: los de los nombres del original.
  std::vector<std::string> argument_types;
  /// Los ejemplos de INSLV y la precondición y el efecto, fusionados.
  rules::Examples logic;
  rules::Examples numeric;
  rules::Fused fused;
};

/// Un dominio aprendido: la jerarquía de tipos y las acciones, por su nombre.
struct LearnedDomain {
  plantraces::TypeHierarchy hierarchy;
  std::vector<LearnedAction> actions;
};

/// El aprendizaje del PlanMiner original de principio a fin, con la configuración original de cada
/// parte: para cada acción, en orden, la tabla de ejemplos; los ejemplos lógicos y sus reglas; los
/// efectos y las relaciones numéricas, los ejemplos numéricos (discretizados con std::rand) y sus
/// reglas; y la fusión.
///
/// Usa el rand() global en el mismo orden que el original: con srand(1) antes (el estado inicial de
/// un programa), da los mismos ejemplos y reglas.
LearnedDomain learn_original(const std::vector<plantraces::Trace>& traces);

/// El dominio como lo escribe el PlanMiner original (el final de Old/src/bin/mainPlanMiner.cpp),
/// con el nombre en minúsculas:
///
/// - requisitos :strips y :typing (nunca :numeric-fluents, aunque haya funciones);
/// - los tipos agrupados por su padre ("object" si no tienen; un dominio sin tipos queda como
///   "object - object");
/// - los predicados y las funciones en el orden en que aparecen en los ejemplos de las acciones,
///   con los tipos de sus argumentos (las secciones salen aunque queden vacías: basta con que haya
///   relaciones o el instante);
/// - cada acción con sus parámetros y los tipos de la cabecera; en la precondición, los átomos
///   ciertos (los falsos se pierden) y las comparaciones; en el efecto, los átomos, los cambios
///   (increase si su valor es positivo, decrease si no) y las asignaciones. Nada que mencione el
///   instante.
///
/// Dos diferencias con el original, por el formato de las trazas, no por el algoritmo: el original
/// busca la cabecera de cada predicado con su nombre en mayúsculas, como venían sus trazas (aquí,
/// con el nombre tal cual); y con una acción sin parámetros lee fuera de rango (aquí, "()").
std::string write_original(const std::string& name, const LearnedDomain& domain);

}  // namespace pddl
