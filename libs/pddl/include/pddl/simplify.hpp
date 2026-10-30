// La simplificación de las precondiciones aprendidas con los estados de las trazas.
#pragma once

#include <vector>

#include "plantraces/actions.hpp"
#include "plantraces/trace.hpp"
#include "rules/legacy.hpp"

namespace pddl {

/// La precondición fusionada de una acción sin lo que sobra según los estados de las trazas. Que
/// unas condiciones impliquen otra quiere decir que en ningún estado hay objetos (de los tipos con
/// que aparece cada parámetro en transitions, las aplicaciones de la acción) con los que se cumplen
/// unas y la otra no. Sobra
///
/// - una comparación que implican las demás condiciones: (>= (onboard ?1) 1) en debark de
///   zenotravel, con (in ?0 ?1). Además, las comparaciones redundantes empeoran la heurística de
///   los planificadores numéricos;
/// - un átomo equivalente a otro del mismo predicado sobre los mismos parámetros (cada uno implica
///   al otro, por sí solo): la inversa de una relación simétrica, (connected ?1 ?0) con
///   (connected ?0 ?1), o (sum ?4 ?3 ?5) con (sum ?3 ?4 ?5). De (up ?1 ?2) y (down ?2 ?1), los
///   datos no dicen cuál escribe el dominio, y se quedan los dos. Un átomo que solo implican los
///   demás se queda, como (room ?0) con (at-robby ?0): los dominios lo escriben.
///
/// Las condiciones se prueban en orden canónico inverso (por sus parámetros y su nombre), y la que
/// sobra se quita antes de probar la siguiente: de dos equivalentes se queda la de los parámetros
/// en orden, (connected ?0 ?1). Solo se prueban (y solo cuentan) los átomos ciertos y las
/// comparaciones, lo que se escribe en el dominio; las demás condiciones se quedan. Un valor que
/// falta no contradice nada.
std::vector<rules::Literal> drop_implied(std::vector<rules::Literal> precondition,
                                         const std::vector<plantraces::Transition>& transitions,
                                         const std::vector<plantraces::Trace>& traces);

}  // namespace pddl
