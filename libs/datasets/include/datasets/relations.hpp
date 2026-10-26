// Las relaciones numéricas de una acción: comparaciones entre fluentes que se añaden a la tabla
// como atributos lógicos, de los que salen las precondiciones numéricas.
#pragma once

#include <string>
#include <vector>

#include "datasets/effects.hpp"
#include "datasets/table.hpp"
#include "plantraces/actions.hpp"
#include "regression/expression.hpp"

namespace datasets {

enum class Comparison { equal, less, greater };

enum class OperandKind {
  /// Una expresión sobre los fluentes (un fluente solo o la expresión de un efecto), con los
  /// valores del estado de la fila.
  expression,
  /// Lo que baja un fluente en la aplicación (f - f', el cambio con el signo cambiado): el mismo
  /// valor en la fila de antes y en la de después. Es con lo que compara el original.
  fall,
};

/// Con qué se compara el fluente de una relación.
struct Operand {
  OperandKind kind = OperandKind::expression;
  /// En fall, la variable 0: el fluente.
  regression::Expression expression;
  /// Los fluentes de las variables de la expresión, por su índice.
  std::vector<plantraces::LiftedAtom> variables;
};

/// Un fluente como operando.
Operand fluent_operand(const plantraces::LiftedAtom& fluent);

/// "(capacity ?0)", "(* (distance ?1 ?2) (slow-burn ?0))" o "(- (fuel ?0) (fuel ?0)')".
std::string to_string(const Operand& operand);

/// El valor del operando en cada fila de la tabla (NaN, desconocido).
std::vector<double> values(const Table& table, const Operand& operand);

/// Una relación (left comparison right) y su valor en cada fila de la tabla. left es un fluente o
/// el instante.
struct Relation {
  Comparison comparison = Comparison::equal;
  Attribute left;
  Operand right;
  /// column[fila]: 1 si se cumple, 0 si no; NaN, desconocida (falta alguno de los valores).
  std::vector<double> column;
};

/// "(< (fuel ?0) (capacity ?0))", "(> (fuel ?0) (* (distance ?1 ?2) (slow-burn ?0)))" o, con el
/// instante, "(> time (- (fuel ?0) (fuel ?0)'))".
std::string to_string(const Relation& relation);

/// Qué relaciones se generan. Cada mejora sobre el original es una opción, para poder medirla por
/// separado: original() es el PlanMiner original y RelationOptions{}, el mejorado.
struct RelationOptions {
  /// Cada fluente se compara con las expresiones de los efectos numéricos de la acción, incluida
  /// la del suyo, con los valores del estado de cada fila: (>= (fuel ?0) (* (distance ?1 ?2)
  /// (slow-burn ?0))), o con una constante, (>= (value ?0) 1). Si no, como en el original, con lo
  /// que baja cada uno de los otros fluentes que cambian.
  bool effect_expressions = true;
  /// Se compara cada pareja en la que algún lado cambia, en cualquier orden, y cambiar es hacerlo
  /// en más de una proporción 1 - max_unchanged de las aplicaciones en que se conoce (el modelo
  /// nulo de los efectos), para que el ruido no haga cambiar a las constantes. Si no, como en el
  /// original, cada fluente que cambia en alguna aplicación con los que van detrás de él en la
  /// tabla.
  bool changing_pairs = true;
  double max_unchanged = 0.75;
  /// Se descartan las relaciones que valen lo mismo en todas las filas en que se conocen. Si no,
  /// como en el original, las que valen lo mismo que la primera fila, aunque sea desconocida: si lo
  /// es, la relación se queda.
  bool known_constants = true;
  /// El instante es un atributo numérico más, como en el original: se compara (siempre a la
  /// izquierda) con los fluentes que van detrás de él y con lo que baja cada fluente que cambia.
  /// Si no, no hay relaciones con el instante, que no es un fluente. El original las descartaba al
  /// escribir el dominio, pero cuentan para lo que aprende INSLV.
  bool time = false;

  static RelationOptions original();
};

/// Las relaciones numéricas de la acción de la tabla. changes son los efectos numéricos de la
/// acción (numeric_effects), de los que se toman las expresiones; el original no los usa.
///
/// Cada fluente se compara (=, < y >) con otros fluentes y con las expresiones de los efectos,
/// según las opciones; en ese orden, y cada uno en el orden de la tabla. Una expresión que es un
/// solo fluente no se usa (ya se compara con los demás fluentes), y una que está en dos efectos se
/// usa una vez. Se descartan las relaciones que no se conocen en ninguna fila y las constantes.
///
std::vector<Relation> numeric_relations(const Table& table,
                                        const std::vector<FluentChange>& changes,
                                        const RelationOptions& options = {});

}  // namespace datasets
