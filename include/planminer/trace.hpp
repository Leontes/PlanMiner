// Trazas de plan en el formato de PDDLDomains (pddldomains-trace, versión 1): los estados por los
// que pasa la ejecución de un plan, tal como se observan (con huecos y ruido), y las acciones.
// El formato está descrito en vendor/PDDLDomains/docs/es/trazas.md.
#pragma once

#include <compare>
#include <cstddef>
#include <filesystem>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace planminer {

/// Un átomo instanciado: (nombre arg1 arg2 ...). También se usa para los términos de los fluentes
/// numéricos, como (fuel plane1).
struct GroundAtom {
  std::string name;
  std::vector<std::string> arguments;

  auto operator<=>(const GroundAtom&) const = default;
};

/// Una acción instanciada del plan, con su paso (desde 1) y su instante.
struct GroundAction {
  std::size_t step = 0;
  double time = 0.0;
  std::string name;
  std::vector<std::string> arguments;
};

/// Lo que se observa de un estado.
///
/// Un átomo que no está entre los ciertos ni entre los desconocidos es falso. Las trazas de mundo
/// abierto listan además los falsos (false_atoms); las de mundo cerrado, no.
struct Observation {
  std::size_t step = 0;
  double time = 0.0;
  /// El estado falta entero: no se sabe nada de él.
  bool missing = false;
  std::set<GroundAtom> true_atoms;
  std::set<GroundAtom> false_atoms;
  std::set<GroundAtom> unknown_atoms;
  /// Los fluentes numéricos observados, incluidos los constantes.
  std::map<GroundAtom, double> values;
  /// Los fluentes numéricos con valor que no se observan.
  std::set<GroundAtom> unknown_values;

  /// El valor del átomo; nullopt si es desconocido o el estado falta.
  std::optional<bool> truth(const GroundAtom& atom) const;
  /// El valor del fluente; nullopt si no se observa.
  std::optional<double> value(const GroundAtom& term) const;
};

enum class World { closed, open };

struct Trace {
  /// Los nombres que declaran el dominio y el problema.
  std::string domain;
  std::string problem;
  /// El tipo declarado de cada objeto y constante: uno, o varios con either.
  std::map<std::string, std::vector<std::string>> objects;
  World world = World::closed;
  /// states[i] es el estado i (0, el inicial) y actions[i] la acción que lleva del estado i al
  /// i + 1.
  std::vector<Observation> states;
  std::vector<GroundAction> actions;
};

class TraceError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

/// Lee un átomo con la sintaxis de las trazas: "(nombre arg1 arg2 ...)". Lanza TraceError si no
/// la tiene.
GroundAtom parse_atom(std::string_view text);

/// El átomo con la sintaxis de las trazas.
std::string to_string(const GroundAtom& atom);

/// Lee una traza del texto JSON. source es el nombre que se da a la traza en los mensajes de
/// error.
///
/// Lanza TraceError si el texto no es JSON, no es una traza de PDDLDomains de la versión 1 o no
/// es coherente: pasos fuera de orden, una acción por estado que no sea el inicial, objetos que no
/// están declarados o átomos con más de un valor.
Trace parse_trace(std::string_view json, std::string_view source = "<traza>");

/// Lee una traza de un fichero (ver parse_trace).
Trace read_trace(const std::filesystem::path& file);

}  // namespace planminer
