// Los conjuntos de ejemplos de una acción para el aprendizaje de reglas (INSLV) y la
// interpretación de las reglas aprendidas.
#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "datasets/discretization.hpp"
#include "datasets/effects.hpp"
#include "datasets/relations.hpp"
#include "datasets/table.hpp"
#include "inslv/inslv.hpp"
#include "regression/search.hpp"

namespace rules {

enum class AttributeKind {
  /// Un átomo: True o False.
  atom,
  /// El valor de un fluente o del instante, discretizado.
  value,
  /// Una relación numérica: True o False.
  relation,
  /// El signo del cambio de un fluente o del instante: NA (antes), POS o NEG (después). Es como el
  /// original aprende el sentido de los efectos numéricos.
  change,
};

/// Lo que representa un atributo de los ejemplos.
struct Attribute {
  AttributeKind kind = AttributeKind::atom;
  /// atom: el átomo; value y change: el fluente o el instante; relation: su lado izquierdo.
  datasets::Attribute term;
  /// relation: la relación (sin su columna).
  std::optional<datasets::Relation> relation;
  /// change: la expresión del cambio que encontró la regresión y los fluentes de sus variables.
  std::optional<regression::Expression> expression;
  std::vector<plantraces::LiftedAtom> variables;
};

/// "(on ?0 ?1)", "value (fuel ?0)", "(< (fuel ?0) (capacity ?0))" o "change (fuel ?0) 1".
std::string to_string(const Attribute& attribute);

/// Un conjunto de ejemplos para INSLV y lo que representa cada uno de sus atributos
/// (attributes[i] es data.atributos[i]). La clase es PreState en las filas de antes de la acción
/// y PostState en las de después.
struct Examples {
  inslv::Datos data;
  std::vector<Attribute> attributes;
};

/// Cómo se construyen los ejemplos. original() es el PlanMiner original.
struct ExampleOptions {
  /// Los ejemplos tienen el signo del cambio de cada fluente (y del instante) que cambia: es como
  /// el original aprende el sentido de los efectos numéricos. Si no, los efectos numéricos salen
  /// directamente de la regresión (numeric_effects), con su forma y su signo.
  bool changes = false;
  /// El instante es un atributo numérico más (su valor y, con changes, su cambio), como en el
  /// original. Las relaciones con el instante dependen de las opciones de las relaciones.
  bool time = false;
  /// La búsqueda de la expresión del cambio del instante, si cuenta. Sin expresión, no hay
  /// atributo de su cambio.
  regression::Options time_search;

  static ExampleOptions original();
};

/// Los ejemplos de los átomos de la acción, en el orden de la tabla: el conjunto lógico del
/// original.
Examples logic_examples(const datasets::Table& table);

/// Los valores raros de los átomos, desconocidos: en cada átomo y en cada clase (las filas de antes
/// y las de después de la acción), el valor que tiene menos de threshold de las filas de la clase
/// en que se conoce pasa a desconocido. Es lowFreq, de la rama ExperimentosRuido del PlanMiner
/// original, para el ruido: para INSLV, un desconocido cumple cualquier condición, así que una
/// condición que el ruido contradice en pocas filas deja de costar bondad; y la validación de la
/// fusión solo cuenta los valores conocidos.
///
/// Una diferencia con lowFreq: las filas que se quedan sin ningún valor conocido no se quitan,
/// porque las de antes y las de después van emparejadas.
void hide_rare_values(Examples& examples, double threshold);

/// Los ejemplos numéricos de la acción, como los construye el original (to_table y discretize):
/// los valores de los fluentes (y del instante) en el orden de la tabla, discretizados con random;
/// las relaciones; y, con options.changes, el signo del cambio de cada fluente (y del instante) que
/// cambia y cuyo efecto tiene expresión, en el orden de la tabla.
///
/// changes son los efectos numéricos de la acción (numeric_effects) y relations, sus relaciones
/// (numeric_relations). El cambio vale 0 en las filas de antes y, en las de después, 1 si el valor
/// sube y -1 si no (también si no cambia), como en el original. Sus etiquetas son NA y, si
/// aparecen, POS y NEG.
///
/// El original discretiza estos ejemplos después de aprender las reglas de los lógicos, y los dos
/// pasos usan el rand() global: para reproducir sus particiones hay que seguir ese orden y pasar
/// como random el generador que recibe INSLV en Opciones::azar (como pddl::learn).
///
/// Una diferencia buscada con el original: un fluente que cambia y cuyo efecto no tiene expresión
/// no tiene atributo de su cambio. El original añadía la columna sin su nombre y desalineaba el
/// conjunto de ejemplos.
Examples numeric_examples(const datasets::Table& table,
                          const std::vector<datasets::FluentChange>& changes,
                          const std::vector<datasets::Relation>& relations,
                          const datasets::Random& random, const ExampleOptions& options = {});

/// Una condición de una regla: el atributo (su índice en Examples::attributes) y su valor, como lo
/// interpreta el original (SplitAntecedent): True o False en los átomos y las relaciones, y el
/// centro de la etiqueta truncado a entero en los valores y los cambios.
struct Condition {
  std::size_t attribute = 0;
  std::variant<bool, int> value;

  bool operator==(const Condition&) const = default;
};

/// Una regla: describe los estados de después de la acción (post) o los de antes, con las
/// condiciones en el orden de los atributos. covered son los ejemplos que cubre.
struct Rule {
  bool post = false;
  int covered = 0;
  std::vector<Condition> conditions;

  bool operator==(const Rule&) const = default;
};

/// Las reglas que aprende INSLV con los ejemplos, interpretadas como en el original: solo cuentan
/// los atributos de hasta dos etiquetas, y en cada uno, cada etiqueta que admite la regla es una
/// condición. Sin atributos, no hay reglas.
std::vector<Rule> learn(const Examples& examples, const inslv::Opciones& options = {});

}  // namespace rules
