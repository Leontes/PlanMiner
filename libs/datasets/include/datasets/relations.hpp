// Las relaciones numéricas de una acción: comparaciones entre fluentes que se añaden a la tabla
// como atributos lógicos, de los que salen las precondiciones numéricas.
#pragma once

#include <string>
#include <vector>

#include "datasets/table.hpp"
#include "plantraces/actions.hpp"

namespace datasets {

enum class Comparison { equal, less, greater };

/// Con qué se compara el fluente de una relación.
enum class OperandKind {
  /// Otro fluente, en el estado de la fila.
  fluent,
  /// Lo que baja otro fluente en la aplicación (f - f', el cambio con el signo cambiado): el
  /// mismo valor en la fila de antes y en la de después.
  fall,
};

/// Una relación (left comparison right) y su valor en cada fila de la tabla.
struct Relation {
  Comparison comparison = Comparison::equal;
  plantraces::LiftedAtom left;
  OperandKind right_kind = OperandKind::fluent;
  plantraces::LiftedAtom right;
  /// columns[fila]: 1 si se cumple, 0 si no; NaN, desconocida (falta alguno de los valores).
  std::vector<double> column;
};

/// "(< (fuel ?0) (capacity ?0))" o, si se compara con lo que baja un fluente,
/// "(> (total-fuel-used) (- (fuel ?0) (fuel ?0)'))".
std::string to_string(const Relation& relation);

/// Las relaciones numéricas de la acción de la tabla, como las genera el PlanMiner original
/// (el final de to_table en Old/src/PlanTraces/Utils.cpp).
///
/// Para cada fluente a que cambia en alguna aplicación (en la que se conoce antes y después),
/// en el orden de la tabla:
/// - las comparaciones =, < y > con cada fluente que va detrás de él en la tabla;
/// - las comparaciones =, < y > con lo que baja cada uno de los otros fluentes que cambian.
///
/// Se descartan las relaciones que no se conocen en ninguna fila y las que valen lo mismo que en
/// la primera fila en todas las que se conocen. Como en el original, si no se conoce la primera
/// fila, la relación se queda.
///
/// Una diferencia buscada con el original: el instante no es un fluente, y no hay relaciones con
/// él (el original las generaba y las descartaba al escribir el dominio).
std::vector<Relation> numeric_relations(const Table& table);

}  // namespace datasets
