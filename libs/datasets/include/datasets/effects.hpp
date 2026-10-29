// Los efectos numéricos de una acción, con la regresión simbólica.
#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "datasets/table.hpp"
#include "plantraces/actions.hpp"
#include "regression/search.hpp"

namespace datasets {

enum class EffectKind {
  increase,
  decrease,
  assign,
  /// Solo el valor absoluto del cambio: la formulación original, en la que el signo lo decide
  /// después INSLV.
  magnitude,
};

/// Un efecto numérico sobre un fluente: (increase f E), (decrease f E) o (assign f E).
struct NumericEffect {
  plantraces::LiftedAtom fluent;
  EffectKind kind = EffectKind::assign;
  regression::Expression expression;
  /// Los fluentes de las variables de la expresión, por su índice.
  std::vector<plantraces::LiftedAtom> variables;
  /// El MAPE (en %) del cambio que explica.
  double error = 0.0;
};

/// "(decrease (fuel ?0) (* (distance ?1 ?2) (slow-burn ?0)))".
std::string to_string(const NumericEffect& effect);

/// Cómo se plantea la regresión. Cada mejora sobre el original es una opción, para poder medirla
/// por separado: original() es la formulación del PlanMiner original y EffectOptions{}, la
/// mejorada.
struct EffectOptions {
  /// Las variables son los fluentes del estado de antes, incluido el propio (la semántica de
  /// PDDL). Si no, los del estado de después, sin el propio, como en el original.
  bool before_state = true;
  /// Los objetivos son las tres formas de efecto, con signo: increase (f' - f), decrease (f - f')
  /// y assign (f'). Una asignación f + E o f - E se escribe como increase o decrease de E. Si no,
  /// el objetivo es |f' - f|, como en el original.
  bool effect_forms = true;
  /// Un fluente que no cambia en la mayoría de las aplicaciones en que se conoce (en una
  /// proporción de al menos max_unchanged) no tiene efecto, aunque el ruido lo altere en alguna.
  /// Si no, cualquier cambio cuenta, como en el original.
  bool null_model = true;
  double max_unchanged = 0.75;
  /// Las operaciones entre constantes de la expresión se sustituyen por su valor
  /// (regression::fold_constants): (* 10 3) pasa a 30. Si no, como en el original, la expresión
  /// queda como la construye la búsqueda.
  bool fold_constants = true;
  /// La búsqueda.
  regression::Options search;

  static EffectOptions original();
};

/// Lo que se ha buscado para un fluente que cambia en alguna aplicación de la acción.
struct FluentChange {
  plantraces::LiftedAtom fluent;
  /// El efecto, si se ha encontrado una expresión.
  std::optional<NumericEffect> effect;
  /// La búsqueda (expansiones, límites).
  regression::Result search;
};

/// Los efectos numéricos de la acción de la tabla: para cada fluente que cambia, la expresión de
/// menor coste de su cambio. Una aplicación cuenta si se conoce el fluente antes y después.
/// Los fluentes que no cambian (o que el modelo nulo da por no cambiados) no aparecen.
std::vector<FluentChange> numeric_effects(const Table& table, const EffectOptions& options = {});

}  // namespace datasets
