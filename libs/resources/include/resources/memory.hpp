// La memoria del proceso y la disponible, y el límite duro de memoria.
//
// Es lo mismo que pyplanner.search.memory, en PyPlanner, y por lo mismo: en WSL, Windows limita
// la memoria de toda la máquina virtual y, si un proceso la agota, WSL entero se congela. Por eso
// todo lo que puede crecer sin control tiene dos límites:
//
// - el blando lo comprueba el propio algoritmo de vez en cuando (por ejemplo, el A* de la
//   regresión simbólica con la memoria residente) y para limpiamente;
// - el duro es RLIMIT_DATA (limit_process): al pasarlo, cualquier reserva de memoria falla con
//   std::bad_alloc. Se usa RLIMIT_DATA y no RLIMIT_AS porque solo cuenta la memoria de datos
//   (montículo y reservas anónimas), que sigue de cerca a la residente.
//
// Se leen /proc/self/statm, /proc/self/status y /proc/meminfo (Linux, incluido WSL). En otros
// sistemas las medidas son nullopt y no se fija ningún límite.
//
// Con AddressSanitizer tampoco: su asignador reserva memoria virtual que RLIMIT_DATA cuenta, y con
// el límite no puede trabajar. En su lugar, ASAN_OPTIONS=hard_rss_limit_mb=<MB> (el preset asan lo
// pone en los tests).
#pragma once

#include <optional>

namespace resources {

/// Margen del límite duro sobre el blando: el mayor de los dos.
inline constexpr double hard_margin_mb = 512;
inline constexpr double hard_margin_fraction = 0.25;

/// La parte de la memoria disponible que usa un proceso por defecto.
inline constexpr double default_memory_fraction = 0.8;

/// La memoria residente del proceso, en MB.
std::optional<double> resident_memory_mb();

/// La memoria de datos del proceso (VmData, lo que cuenta RLIMIT_DATA), en MB.
std::optional<double> data_memory_mb();

/// La memoria disponible en el sistema (MemAvailable), en MB.
std::optional<double> available_memory_mb();

/// La memoria por defecto de un proceso: el 80 % de la disponible.
std::optional<double> default_memory_limit_mb();

/// El límite duro para un proceso con límite blando soft (MB): el mayor entre soft y lo que ya
/// ocupa, más un margen.
double hard_limit_mb(double soft);

/// Si se puede fijar el límite duro: en Linux, sin AddressSanitizer.
bool hard_limit_supported();

/// Fija el límite duro de memoria de datos (RLIMIT_DATA) de este proceso y de los que cree
/// después. Si ya había uno menor, se queda ese: un proceso no puede subir su límite. Devuelve
/// true si se ha aplicado (ver hard_limit_supported).
bool limit_process(double megabytes);

}  // namespace resources
