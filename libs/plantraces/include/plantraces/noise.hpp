// El ruido de las trazas: cuánto hay en cada observación y lo que puede explicar.
#pragma once

#include <cstddef>
#include <vector>

#include "plantraces/trace.hpp"

namespace plantraces {

/// Lo que cambia fuera de las acciones: de las observaciones de los estados de antes y de después
/// de cada aplicación (conocidas en los dos) de átomos y fluentes con argumentos, ninguno de ellos
/// de la acción, la proporción que cambia. Sin ruido es 0: una acción solo cambia lo de sus
/// argumentos. Con ruido p en cada estado (pddldomains trace --noise), cerca de 2p.
///
/// Los átomos y fluentes sin argumentos no cuentan: una acción puede cambiarlos.
double frame_changes(const std::vector<Trace>& traces);

/// El ruido de cada observación que da frame_changes: si cada observación se altera con
/// probabilidad p, independiente en cada estado, una observación cambia entre dos estados con
/// probabilidad c = 2p(1 - p), y p = (1 - sqrt(1 - 2c)) / 2. Desde c = 0.5, 0.5.
double observation_noise(double frame_changes);

/// La cota de lo que explica el azar en una binomial: el menor m tal que P(X > m) <= alpha, con
/// X ~ Binomial(n, p). Con p 0, 0.
std::size_t binomial_bound(std::size_t n, double p, double alpha);

}  // namespace plantraces
