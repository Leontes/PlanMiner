// Los nombres que da el PlanMiner original a los atributos y las reglas con esos nombres: la
// fusión de reglas del original trabaja sobre ellos, como cadenas.
#pragma once

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
///   fluente, la expresión de su cambio (o el fluente, si no tiene);
/// - un cambio: "DELTA - ( fuel ... ) <expresión> ".
std::string legacy_name(const Attribute& attribute, const Examples& examples,
                        const std::vector<std::string>& types);

/// Las reglas con los nombres del original: los ejemplos cubiertos con signo y las condiciones,
/// con True, False o el número.
std::vector<LegacyRule> legacy_rules(const std::vector<Rule>& rules, const Examples& examples,
                                     const std::vector<std::string>& types);

}  // namespace rules
