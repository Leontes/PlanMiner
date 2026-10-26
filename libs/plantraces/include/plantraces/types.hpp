// La jerarquía de tipos de las trazas y los tipos de los parámetros de las acciones y de los
// argumentos de los predicados y fluentes.
#pragma once

#include <map>
#include <string>
#include <vector>

#include "plantraces/trace.hpp"

namespace plantraces {

/// Un tipo y su padre (vacío si no tiene).
struct Type {
  std::string name;
  std::string parent;

  bool operator==(const Type&) const = default;
};

/// La jerarquía de tipos y las cabeceras: el tipo de cada parámetro de cada acción y de cada
/// argumento de cada predicado o fluente, por su posición.
struct TypeHierarchy {
  /// En el orden en que aparecen: los de los parámetros de las acciones, los de los argumentos y
  /// los padres que se crean.
  std::vector<Type> types;
  std::map<std::string, std::vector<std::string>> actions;
  std::map<std::string, std::vector<std::string>> predicates;
};

/// La jerarquía de tipos como la infiere el PlanMiner original (extractTypeHierarchy en
/// Old/src/PlanTraces/Utils.cpp), de los tipos de los objetos de las trazas:
///
/// - Se recogen los tipos de cada parámetro de cada acción en sus aplicaciones, y los de cada
///   argumento de cada predicado o fluente en los estados de antes y de después de cada
///   aplicación (los átomos conocidos y después los valores, en orden), sin repetir.
/// - Hay un conflicto si un mismo parámetro o argumento aparece con dos tipos. Se resuelven en
///   orden: dos tipos sin padre reciben uno nuevo, PARENTTYPE<i> (i, el número del conflicto); si
///   los dos lo tienen y es distinto, se añade el conflicto entre sus padres; si solo uno lo tiene,
///   el otro pasa a tenerlo.
/// - El tipo de un parámetro o argumento con conflicto es el primer antecesor común de su primer
///   tipo y del último con el que choca (vacío si no lo hay); si no, su tipo.
///
/// Se usa el primer tipo de cada objeto, y no cuentan los estados que faltan. Las acciones,
/// predicados y fluentes sin argumentos no tienen cabecera.
TypeHierarchy type_hierarchy(const std::vector<Trace>& traces);

}  // namespace plantraces
