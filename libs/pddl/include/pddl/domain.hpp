// El dominio que aprende PlanMiner de las trazas y su escritura en PDDL.
#pragma once

#include <string>
#include <vector>

#include "datasets/effects.hpp"
#include "datasets/relations.hpp"
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
  /// Los efectos numéricos de la regresión, si no salen de las reglas (sin los atributos de
  /// cambio en los ejemplos).
  std::vector<datasets::NumericEffect> effects;
};

/// Un dominio aprendido: la jerarquía de tipos y las acciones, por su nombre.
struct LearnedDomain {
  plantraces::TypeHierarchy hierarchy;
  std::vector<LearnedAction> actions;
};

/// Cómo se aprende y se escribe el dominio: las opciones de cada parte. original() es el PlanMiner
/// original y Options{}, el mejorado: con las mejoras aprobadas de cada parte y, en lo que aún no
/// se ha decidido (los valores discretizados que recibe INSLV), como el original.
struct Options {
  datasets::EffectOptions effects;
  datasets::RelationOptions relations;
  /// Sin los atributos de cambio (examples.changes), los efectos numéricos salen de la regresión,
  /// con su forma y su signo, y de la fusión solo quedan los efectos sobre átomos: el original
  /// convierte en asignaciones las condiciones sobre valores discretizados del estado de después,
  /// y contradirían a los de la regresión.
  rules::ExampleOptions examples;
  rules::FusionOptions fusion;
  /// El dominio declara :numeric-fluents si tiene funciones. El original no lo declara, y algunos
  /// planificadores (PyPlanner) rechazan el dominio.
  bool numeric_fluents = true;
  /// Las igualdades se escriben con =. El original escribe ==, que no es PDDL.
  bool equality = true;

  static Options original();
};

/// El aprendizaje de principio a fin: para cada acción, en orden, la tabla de ejemplos; los
/// ejemplos lógicos y sus reglas; los efectos y las relaciones numéricas, los ejemplos numéricos
/// (discretizados con std::rand) y sus reglas; y la fusión.
///
/// Usa el rand() global en el mismo orden que el original: con Options::original() y srand(1)
/// antes (el estado inicial de un programa), da los mismos ejemplos y reglas.
LearnedDomain learn(const std::vector<plantraces::Trace>& traces, const Options& options = {});

/// El dominio como lo escribe el PlanMiner original (el final de Old/src/bin/mainPlanMiner.cpp),
/// con el nombre en minúsculas:
///
/// - requisitos :strips y :typing (y, con options.numeric_fluents, :numeric-fluents si hay
///   funciones);
/// - los tipos agrupados por su padre ("object" si no tienen; un dominio sin tipos queda como
///   "object - object");
/// - los predicados y las funciones en el orden en que aparecen en los ejemplos de las acciones,
///   con los tipos de sus argumentos (las secciones salen aunque queden vacías: basta con que haya
///   relaciones o el instante);
/// - cada acción con sus parámetros y los tipos de la cabecera; en la precondición, los átomos
///   ciertos (los falsos se pierden) y las comparaciones (las igualdades, con == o, con
///   options.equality, con =); en el efecto, los átomos, los cambios
///   (increase si su valor es positivo, decrease si no), las asignaciones y los efectos de la
///   regresión. Nada que mencione el instante.
///
/// Dos diferencias con el original, por el formato de las trazas, no por el algoritmo: el original
/// busca la cabecera de cada predicado con su nombre en mayúsculas, como venían sus trazas (aquí,
/// con el nombre tal cual); y con una acción sin parámetros lee fuera de rango (aquí, "()").
std::string write(const std::string& name, const LearnedDomain& domain,
                  const Options& options = {});

}  // namespace pddl
