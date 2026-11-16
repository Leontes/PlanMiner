// El ruido de las trazas: cuánto hay en cada observación, lo que puede explicar y su suavizado
// con la secuencia de los estados.
#pragma once

#include <array>
#include <cstddef>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "plantraces/actions.hpp"
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

/// Lo que cambia fuera de las acciones, solo en los valores: como frame_changes, de los fluentes
/// con argumentos, ninguno de ellos de la acción. Con ruido p en cada valor (pddldomains trace
/// --noise), cerca de 1 - (1 - p)^2: dos observaciones difieren si alguna está alterada.
double value_changes(const std::vector<Trace>& traces);

/// El ruido de cada valor que da value_changes: p = 1 - sqrt(1 - c). Desde c = 1, 1.
double value_noise(double value_changes);

/// La cota de lo que explica el azar en una binomial: el menor m tal que P(X > m) <= alpha, con
/// X ~ Binomial(n, p). Con p 0, 0.
std::size_t binomial_bound(std::size_t n, double p, double alpha);

/// Una acción y un átomo sobre sus parámetros: ("move", (at ?0 ?1)).
using ActionAtom = std::pair<std::string, LiftedAtom>;

/// El ruido que queda en las columnas de las tablas de ejemplos: de cada acción y átomo sobre sus
/// parámetros, la probabilidad media de que esté mal una observación de antes de la acción ([0])
/// y de después ([1]).
using ColumnNoise = std::map<ActionAtom, std::array<double, 2>>;

/// El suavizado temporal de los átomos, en las trazas. Las tablas de ejemplos separan cada
/// aplicación de las de antes y de después; pero el ruido es independiente en cada estado, y el
/// valor de un átomo persiste mientras no lo cambie una acción. p es el ruido de cada observación
/// (observation_noise). En cada traza:
///
/// 1. Un átomo con argumentos solo puede cambiar con una acción que tenga todos sus objetos entre
///    sus argumentos (suponiendo que los efectos no tienen constantes del dominio). Una acción
///    cambia un átomo sobre unos de sus parámetros, como (at ?0 ?1) en move, si, entre los estados
///    de antes y de después, cambia más de lo que explica el ruido: en más de
///    binomial_bound(n, 2p(1 - p), effect_alpha) de las n aplicaciones en que se conoce en los
///    dos. Con un alpha alto, ante la duda, es un efecto: tomar un efecto por ruido suavizaría por
///    encima de un cambio y destruiría datos; lo contrario solo pierde el suavizado en ese punto.
/// 2. Las acciones que pueden cambiar el átomo (sus efectos, las que lo tienen sobre un objeto que
///    ocupa varios parámetros y las que no tienen datos) cortan sus estados en tramos. Dentro de un
///    tramo, su valor es el mismo.
/// 3. En cada tramo, con d la diferencia entre las observaciones ciertas y las falsas: con d >= 2,
///    todas pasan al valor de la mayoría, que se equivoca con probabilidad
///    1 / (1 + ((1 - p) / p)^d); con d <= 1 y dos observaciones o más, a desconocidas (la mayoría
///    no es más fiable que una observación sola, y si se equivocara, se equivocarían todas juntas);
///    con una sola, se queda como está, con su ruido p. Las desconocidas no se rellenan.
///
/// Los átomos sin argumentos (cualquier acción puede cambiarlos) y los fluentes no se suavizan.
/// Devuelve el ruido que queda: en las columnas de cada acción, la media de esas probabilidades en
/// las observaciones conocidas (p si no tiene ninguna).
ColumnNoise smooth_atoms(std::vector<Trace>& traces, double p, double effect_alpha = 0.3);

/// El suavizado temporal de los fluentes con argumentos, en las trazas: como smooth_atoms, con p
/// el ruido de cada valor (value_noise). Las acciones que cambian un fluente sobre sus parámetros
/// son las que lo cambian en más de binomial_bound(n, 1 - (1 - p)^2, effect_alpha) de sus n
/// aplicaciones, y cortan sus estados en tramos. En cada tramo, los valores no alterados son
/// todos iguales y los alterados, casi siempre distintos entre sí: si un valor sale al menos dos
/// veces y más que cualquier otro (la moda), todas las observaciones pasan a él; si no (también
/// con una sola observación, que nada corrobora), a desconocidas. Así, lo que queda conocido está
/// casi siempre bien: en las trazas de prueba de zenotravel con ruido no queda ningún valor mal.
///
/// Los fluentes sin argumentos (cualquier acción puede cambiarlos) no se suavizan.
void smooth_values(std::vector<Trace>& traces, double p, double effect_alpha = 0.3);

}  // namespace plantraces
