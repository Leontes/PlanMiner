// Las acciones de las trazas: sus aplicaciones agrupadas por nombre y los átomos que se pueden
// expresar con sus parámetros.
#pragma once

#include <compare>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "plantraces/trace.hpp"

namespace plantraces {

/// Lo que se observa de un predicado o de un fluente numérico en las trazas: su aridad y, para
/// cada argumento, los tipos de los objetos con que aparece.
struct Signature {
  std::size_t arity = 0;
  std::vector<std::set<std::string>> types;
};

/// Los predicados y los fluentes numéricos de las trazas, con su firma. Un predicado se observa
/// en las listas de ciertos, falsos y desconocidos; un fluente, en las de valores y desconocidos.
struct Vocabulary {
  std::map<std::string, Signature> predicates;
  std::map<std::string, Signature> fluents;
};

/// El vocabulario de las trazas. Lanza TraceError si un predicado o un fluente aparece con
/// aridades distintas.
Vocabulary vocabulary(const std::vector<Trace>& traces);

/// Una aplicación de una acción en una traza: la acción instanciada y los estados de antes y de
/// después. Apunta a la traza, que tiene que existir mientras se use.
struct Transition {
  const Trace* trace = nullptr;
  const GroundAction* action = nullptr;
  const Observation* before = nullptr;
  const Observation* after = nullptr;
};

/// Las aplicaciones de cada acción, por su nombre, en el orden de las trazas y de sus pasos.
std::map<std::string, std::vector<Transition>> group_by_action(const std::vector<Trace>& traces);

/// Lo que cambia fuera de las acciones: de las observaciones de los estados de antes y de después
/// de cada aplicación (conocidas en los dos) de átomos y fluentes con argumentos, ninguno de ellos
/// de la acción, la proporción que cambia. Sin ruido es 0: una acción solo cambia lo de sus
/// argumentos. Con ruido p en cada estado (pddldomains trace --noise), cerca de 2p.
///
/// Los átomos y fluentes sin argumentos no cuentan: una acción puede cambiarlos.
double frame_changes(const std::vector<Trace>& traces);

/// Un átomo o un término numérico sobre los parámetros de una acción: (on ?0 ?1). parameters
/// tiene, para cada argumento, el índice del parámetro.
struct LiftedAtom {
  std::string name;
  std::vector<std::size_t> parameters;

  auto operator<=>(const LiftedAtom&) const = default;
};

/// El átomo con los parámetros como ?0, ?1...: "(on ?0 ?1)".
std::string to_string(const LiftedAtom& atom);

/// Un átomo instanciado y su forma sobre los parámetros de la acción.
struct Expressible {
  GroundAtom ground;
  LiftedAtom lifted;
};

/// Los átomos instanciados de los símbolos que se pueden expresar con los parámetros de la
/// acción: todos sus argumentos son argumentos de la acción y sus tipos encajan con los que se
/// observan en ese argumento del símbolo. Ordenados como GroundAtom.
///
/// Como en el PlanMiner original, un objeto que aparece en varios argumentos de la acción se
/// expresa siempre con el primer parámetro que lo tiene.
std::vector<Expressible> expressible(const std::map<std::string, Signature>& symbols,
                                     const Transition& transition);

}  // namespace plantraces
