// Los nombres que da el PlanMiner original a los atributos y las reglas con esos nombres: la
// fusión de reglas del original trabaja sobre ellos, como cadenas.
#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include "datasets/table.hpp"
#include "regression/expression.hpp"
#include "rules/examples.hpp"

namespace rules {

/// Una condición con los nombres del original: el atributo y su valor ("True", "False" o un
/// número).
struct Literal {
  std::string attribute;
  std::string value;

  bool operator==(const Literal&) const = default;
};

/// Una regla con los nombres del original: los ejemplos que cubre, en negativo si describe el
/// estado de antes, y sus condiciones.
struct LegacyRule {
  int covered = 0;
  std::vector<Literal> conditions;

  bool operator==(const LegacyRule&) const = default;
};

/// El nombre que da el original a un átomo, un fluente o el instante:
/// "( on ?param_0 ~ block ?param_1 ~ block )" o "TIMESTAMP". types son los tipos de los
/// argumentos de la primera aplicación de la acción.
std::string legacy_label(const datasets::Attribute& term, const std::vector<std::string>& types);

/// Una expresión como la escribe el original, con los nombres de los fluentes:
/// "( * ( distance ?param_1 ~ city ?param_2 ~ city ) ( slow-burn ?param_0 ~ aircraft ) )".
std::string legacy_formula(const regression::Expression& expression,
                           const std::vector<plantraces::LiftedAtom>& variables,
                           const std::vector<std::string>& types);

/// El nombre que da el original a un atributo de los ejemplos:
///
/// - un átomo, un valor: el del átomo, el fluente o el instante;
/// - una relación: "opEQ - ( a ... )  ( b ... )", "opLT - ..." u "opGT - ..."; con lo que baja un
///   fluente, la expresión de su cambio (o el fluente, si no tiene); con una expresión, la
///   expresión con el formato del original;
/// - un cambio: "DELTA - ( fuel ... ) <expresión> ".
std::string legacy_name(const Attribute& attribute, const Examples& examples,
                        const std::vector<std::string>& types);

/// Los valores de los atributos de los ejemplos de una acción, por su nombre del original: con
/// ellos se comprueban las condiciones de las reglas.
struct Evidence {
  /// Las aplicaciones de la acción: las filas de antes y, detrás, las de después.
  std::size_t transitions = 0;
  std::map<std::string, std::vector<double>> columns;
};

/// Los valores de los ejemplos (lógicos y numéricos) de una acción.
Evidence evidence(const std::vector<const Examples*>& examples,
                  const std::vector<std::string>& types);

/// Las reglas con los nombres del original: los ejemplos cubiertos con signo y las condiciones,
/// con True, False o el número.
std::vector<LegacyRule> legacy_rules(const std::vector<Rule>& rules, const Examples& examples,
                                     const std::vector<std::string>& types);

/// Las cotas que se cumplen en los estados de antes de una acción y no en todos los estados de las
/// trazas, como una regla del estado de antes que cubre todos sus ejemplos: son precondiciones que
/// INSLV no ve si valen lo mismo antes y después de la acción, porque datasets::numeric_relations
/// descarta las relaciones constantes (en fly-fast, onboard no cambia, y (> (onboard ?0)
/// (zoom-limit ?0)) es falsa en todas las filas). De las relaciones candidatas
/// (datasets::candidate_relations), una cota x <= y (o x >= y) se queda si
///
/// - se cumple en al menos min_support de los estados de antes en que se conoce;
/// - se alcanza: x = y en alguno de ellos (las coincidencias de los datos, como (< (zoom-limit ?0)
///   (fuel ?0)), dejan siempre holgura);
/// - no es un invariante: no se cumple en algún estado de las trazas con algunos objetos
///   (datasets::invariant).
///
/// numeric son los ejemplos numéricos de la acción (para los nombres) y types, los tipos de los
/// argumentos de su primera aplicación.
LegacyRule contrast_rule(const std::vector<datasets::Relation>& candidates, const Examples& numeric,
                         const std::vector<std::string>& types,
                         const std::vector<plantraces::Trace>& traces, double min_support);

}  // namespace rules
