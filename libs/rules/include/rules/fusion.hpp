// La fusión de las reglas de una acción en su precondición y su efecto.
#pragma once

#include <string>
#include <vector>

#include "rules/legacy.hpp"

namespace rules {

/// La precondición y el efecto de una acción, con los nombres del original.
struct Fused {
  std::vector<Literal> precondition;
  std::vector<Literal> effect;
};

/// La fusión del PlanMiner original (fuseRules en Old/src/Rules/FuseRules.cpp), tal cual, sobre
/// los nombres del original. rules son las reglas de los dos conjuntos de ejemplos de la acción,
/// los lógicos y después los numéricos (legacy_rules); header, el tipo de cada parámetro de la
/// acción (plantraces::type_hierarchy).
///
/// - En los nombres, el tipo de cada parámetro pasa a ser el de la cabecera ("?param_0 ~ ?tipo").
/// - Las condiciones de las reglas del estado de antes, y aparte las del de después, se juntan
///   sumando los ejemplos que cubren. Si un atributo tiene dos valores, gana el primero si cubre
///   más del 5 % de los dos ejemplos (casi siempre); si no, el otro. En el estado de después, el
///   otro nunca gana: se queda también el primero.
/// - Precondición: los átomos tal cual; las relaciones de cada pareja, con una sola comparación,
///   la de la última condición (si es False y dice =, queda =); las comparaciones con un número,
///   como un intervalo, con la frontera escrita con std::to_string. Las condiciones sobre valores
///   discretizados se pierden. Después se quitan las relaciones cuyo lado derecho está contenido
///   en el de otra con el mismo lado izquierdo (como cadena).
/// - Efecto: los átomos; los cambios con expresión, con su signo; los cambios constantes no nulos
///   y los valores discretizados, como cambio o, si solo hay valores, como asignación; sin lo que
///   ya está en la precondición.
///
/// Lo que el original deja indefinido (lecturas fuera de rango): una comparación cuyo lado
/// izquierdo no tiene paréntesis (el instante) llega hasta el final del nombre; y, al buscar la
/// asignación que repite un cambio, un token que falta cuenta como distinto.
Fused fuse_original(std::vector<LegacyRule> rules, const std::vector<std::string>& header);

/// Cómo se fusionan las reglas. Cada corrección del original es una opción, para poder medirla por
/// separado: original() es la fusión del original y FusionOptions{}, la corregida.
struct FusionOptions {
  /// Cada condición de una regla (del estado de antes o del de después) se queda solo si se cumple
  /// en al menos min_support de los ejemplos de su estado en que se conoce. Es la medida del
  /// impacto que preveía el original (measureImpact y significative, que no llegó a usar): INSLV
  /// describe cada estado con varias reglas, cada una de una parte de los ejemplos, y la fusión
  /// las junta como si se cumplieran todas. Las condiciones sobre valores discretizados no se
  /// comprueban.
  bool validate = true;
  double min_support = 0.95;
  /// En un conflicto entre dos valores de un atributo gana el que cubre más ejemplos, antes y
  /// después; con un empate, ninguno. Si no, como el original: el primero si cubre más del 5 %.
  bool majority = true;
  /// Las comparaciones de cada pareja (o de un fluente con un número), ciertas y falsas, se
  /// combinan en una: <, <=, =, >= o >. Si se contradicen o solo dicen «distinto», ninguna. Si no,
  /// como el original: solo cuentan las ciertas, de las relaciones queda la última y de los
  /// números, un intervalo.
  bool combine_comparisons = true;

  static FusionOptions original();
};

/// La fusión con las correcciones de options. evidence son los valores de los ejemplos de la
/// acción (solo hacen falta para validar).
Fused fuse(std::vector<LegacyRule> rules, const std::vector<std::string>& header,
           const Evidence& evidence, const FusionOptions& options = {});

}  // namespace rules
