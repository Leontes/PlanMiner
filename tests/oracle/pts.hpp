// Trazas nuevas en el formato del PlanMiner original (.pts), para pasárselas a Old/.
#pragma once

#include <string>
#include <vector>

#include "plantraces/trace.hpp"

namespace oracle {

/// Las trazas en el formato .pts del PlanMiner original, un plan por traza.
///
/// Cada objeto lleva su tipo (obj - tipo). En cada estado se escriben los átomos conocidos en el
/// orden de GroundAtom, los falsos como (not ...), y después los fluentes con valor. Los
/// desconocidos y los estados que faltan no se escriben: en el formato antiguo, lo que no aparece
/// es desconocido. Por eso, para que el original vea lo mismo, las trazas tienen que ser de
/// mundo abierto.
///
/// Con uppercase, los nombres (acciones, predicados, fluentes, objetos y tipos) van en mayúsculas,
/// como en las trazas del original: su escritura del dominio lo supone.
///
/// Lanza std::invalid_argument si un objeto tiene varios tipos (either): el formato antiguo no
/// los admite.
std::string to_pts(const std::vector<plantraces::Trace>& traces, bool uppercase = false);

}  // namespace oracle
