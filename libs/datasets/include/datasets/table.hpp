// La tabla de ejemplos de una acción: lo que se observa de sus parámetros antes y después de
// cada aplicación.
#pragma once

#include <compare>
#include <cstddef>
#include <string>
#include <vector>

#include "plantraces/actions.hpp"

namespace datasets {

enum class AttributeKind { atom, fluent, time };

/// Una columna de la tabla: un átomo (1 cierto, 0 falso), un fluente numérico (su valor) o el
/// instante del estado.
struct Attribute {
  AttributeKind kind = AttributeKind::atom;
  /// El átomo o el término sobre los parámetros de la acción; vacío en el instante.
  plantraces::LiftedAtom term;

  auto operator<=>(const Attribute&) const = default;
};

/// "(on ?0 ?1)", "fluent (fuel ?0)" o "time".
std::string to_string(const Attribute& attribute);

/// La tabla de ejemplos de una acción.
///
/// Tiene una fila por estado: primero los de antes de cada aplicación y después los de después,
/// en el orden de las aplicaciones. Un estado que falta da una fila en la que solo se conoce el
/// instante. columns[a][fila] es el valor del atributo a; NaN, desconocido.
struct Table {
  std::vector<Attribute> attributes;
  std::vector<std::vector<double>> columns;
  /// Las aplicaciones de la acción: la tabla tiene el doble de filas.
  std::size_t transitions = 0;

  std::size_t rows() const { return 2 * transitions; }
  /// Si la fila es de un estado de después de la acción (la clase, en el aprendizaje).
  bool after(std::size_t row) const { return row >= transitions; }
};

/// La tabla de las aplicaciones de una acción.
///
/// Las columnas son los átomos y los fluentes que se pueden expresar con los parámetros de la
/// acción (plantraces::expressible) y tienen algún valor conocido, más el instante, en el orden
/// en que aparecen al recorrer las filas: en cada estado, primero los átomos, después los
/// fluentes y después el instante, cada uno en el orden de sus átomos instanciados. Es el orden
/// del PlanMiner original.
///
/// Un átomo que no está entre los ciertos ni entre los desconocidos es falso; un fluente sin
/// valor observado es desconocido.
Table build_table(const plantraces::Vocabulary& vocabulary,
                  const std::vector<plantraces::Transition>& transitions);

/// En las aplicaciones en que un objeto ocupa varios parámetros (move ?from ?to con ?from = ?to),
/// los átomos sobre esos parámetros pasan a ser desconocidos, antes y después. El átomo se expresa
/// con el primero de ellos (plantraces::expressible), y un borrado y una adición del mismo átomo se
/// anulan: el efecto no se ve, y la fila de después contradiría al borrado de las demás. Se oculta
/// también la de antes para no desequilibrar las dos clases: un átomo conocido solo antes
/// parecería borrarse. transitions son las de la tabla.
void hide_aliased_atoms(Table& table, const std::vector<plantraces::Transition>& transitions);

}  // namespace datasets
