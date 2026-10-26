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

}  // namespace rules
