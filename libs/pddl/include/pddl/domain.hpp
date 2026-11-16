// El dominio que aprende PlanMiner de las trazas y su escritura en PDDL.
#pragma once

#include <string>
#include <vector>

#include "datasets/effects.hpp"
#include "datasets/relations.hpp"
#include "plantraces/trace.hpp"
#include "plantraces/types.hpp"
#include "rules/examples.hpp"
#include "rules/fusion.hpp"

namespace pddl {

/// Lo que se aprende de una acción.
struct LearnedAction {
  std::string name;
  /// Los tipos de los argumentos de su primera aplicación: los de los nombres del original.
  std::vector<std::string> argument_types;
  /// Los ejemplos de INSLV y la precondición y el efecto, fusionados.
  rules::Examples logic;
  rules::Examples numeric;
  rules::Fused fused;
  /// Los efectos numéricos de la regresión, si no salen de las reglas (sin los atributos de
  /// cambio en los ejemplos).
  std::vector<datasets::NumericEffect> effects;
};

/// Un dominio aprendido: la jerarquía de tipos y las acciones, por su nombre.
struct LearnedDomain {
  plantraces::TypeHierarchy hierarchy;
  std::vector<LearnedAction> actions;
};

/// Cómo se aprende y se escribe el dominio: las opciones de cada parte. original() es el PlanMiner
/// original y Options{}, el mejorado: con las mejoras aprobadas de cada parte y, en lo que aún no
/// se ha decidido (los valores discretizados que recibe INSLV), como el original.
struct Options {
  /// En las aplicaciones en que un objeto ocupa varios parámetros (move ?from ?to con ?from =
  /// ?to), los átomos sobre esos parámetros son desconocidos (datasets::hide_aliased_atoms): un
  /// borrado y una adición del mismo átomo se anulan, así que el efecto no se ve. Si no, como en
  /// el original, esas aplicaciones contradicen el borrado.
  bool aliasing = true;
  datasets::EffectOptions effects;
  datasets::RelationOptions relations;
  /// Sin los atributos de cambio (examples.changes), los efectos numéricos salen de la regresión,
  /// con su forma y su signo, y de la fusión solo quedan los efectos sobre átomos: el original
  /// convierte en asignaciones las condiciones sobre valores discretizados del estado de después,
  /// y contradirían a los de la regresión.
  rules::ExampleOptions examples;
  rules::FusionOptions fusion;
  /// Si las trazas no tienen ruido (plantraces::frame_changes es 0), la validación de la fusión
  /// exige que cada condición se cumpla en todos los ejemplos en que se conoce (min_support 1):
  /// sin ruido, una condición que contradice algún ejemplo es falsa. Si no, con el min_support de
  /// fusion: con datos limpios, (> (value ?0) 1) en decrement pasaba con 21 de 1611 aplicaciones
  /// con valor 1.
  bool exact_without_noise = true;
  /// Si las trazas tienen ruido (plantraces::frame_changes c > 0), en los ejemplos lógicos, el
  /// valor de un átomo que tiene menos de rare_values de las filas de su clase (antes o después de
  /// la acción) pasa a desconocido (rules::hide_rare_values: lowFreq, de la rama ExperimentosRuido,
  /// con 0.05). Así, INSLV puede añadir la condición sin que le cueste bondad y la validación de la
  /// fusión no cuenta esos valores. Con rare_values_per_noise, el umbral es rare_values · c. El
  /// umbral no pasa de 0.5: el valor mayoritario no se borra nunca. Con 0, como el original, nada.
  ///
  /// 2c: con un ruido p por observación, c ≈ 2p, y una condición cierta sale falsa en ≈ p de sus
  /// filas. Con el 5 % fijo de lowFreq, en noise5 solo se limpia la mitad. La mejorada usa el de la
  /// binomial (rare_values_alpha), y este umbral solo si rare_values_alpha es 0.
  double rare_values = 2.0;
  bool rare_values_per_noise = true;
  /// Si es mayor que 0, en lugar de un umbral fijo, el de una binomial con este alpha
  /// (rules::hide_rare_values_binomial): en cada átomo y clase con n filas, el valor que sale en
  /// plantraces::binomial_bound(n, p, alpha) filas o menos, con p el ruido de cada observación
  /// (plantraces::observation_noise de c). Si no, el umbral de rare_values.
  ///
  /// La mejorada usa 1e-3. Un umbral fijo solo acierta con muchas filas (deja los valores raros
  /// que son ciertos: 2c los borra en scanalyzer) o con pocas (borra el ruido, que varía más). En
  /// el banco ruido, de 3493 a 3954 planes válidos STRIPS con ruido; con 1e-4, 3856.
  double rare_values_alpha = 1e-3;
  /// Si las trazas tienen ruido, antes de aprender, el suavizado temporal de los átomos
  /// (plantraces::smooth_atoms): en cada traza, el valor de un átomo persiste entre las acciones
  /// que pueden cambiarlo, y sus observaciones en cada tramo pasan al valor de la mayoría (o, sin
  /// una mayoría clara, a desconocidas). Las tablas separan cada aplicación de las demás; el
  /// suavizado usa la secuencia de la traza para corregir el ruido. El umbral de la binomial
  /// (rare_values_alpha) usa en cada columna el ruido que queda (en las que no se suavizan, como
  /// las de los átomos sin argumentos, el de antes). El ruido (frame_changes) se estima en las
  /// trazas sin suavizar. Si no, como el original.
  ///
  /// La mejorada lo usa. En el banco ruido, los planes válidos STRIPS con ruido pasan de 3954 a
  /// 4228: 92, 92, 92 y 89 % con un 3, 5, 10 y 20 % de ruido (sin ruido, 92 %), frente a 92, 87,
  /// 84 y 78 %; aprender con ruido cuesta un 40 % más. El fallo que corrige: los valores raros que
  /// son ciertos, como un shaker ya agitado cuando se coge en barman, salían con la frecuencia del
  /// ruido, el umbral los borraba e INSLV añadía la precondición contraria. Supone que los efectos
  /// no tienen constantes del dominio: en woodworking, que las tiene, la F baja un poco.
  bool smooth_atoms = true;
  /// Si las trazas tienen ruido en los valores (plantraces::value_changes > 0), antes de aprender,
  /// el suavizado temporal de los fluentes con argumentos y de las constantes sin argumentos
  /// (plantraces::smooth_values): en cada tramo entre las acciones que pueden cambiar el fluente,
  /// la moda de sus valores; sin una moda clara o con una sola observación, desconocidos. Lo que
  /// queda conocido está casi siempre bien. El HRC (smooth_fluents) se queda con los demás
  /// fluentes: con los datos ya limpios, agrupaba valores ciertos y parecidos, y pasaba una
  /// constante con ruido, como (max_int), a la media de su cluster, que no es exactamente el valor.
  /// Si no, como el original.
  ///
  /// La mejorada usa las cuatro opciones de los numéricos con ruido (smooth_values,
  /// exact_comparisons, noisy_accumulators y noisy_null_model). En el banco ruido, con el HRC,
  /// llevan la validez de los numéricos con un 3, 5, 10 y 20 % de ruido de 74, 66, 87 y 65 % a 97,
  /// 97, 97 y 94 %, sin cambiar la de STRIPS.
  bool smooth_values = true;
  /// Con smooth_values, las cadenas de tramos (plantraces::smooth_values con chains): una acción
  /// que cambia el fluente siempre en lo mismo, como board +1 a (onboard ?a), no corta su tramo.
  /// Sin ellas, el valor entre dos board seguidos está solo en su tramo y se oculta, y lo que se
  /// oculta así no es al azar: (<= (onboard ?1) 1) pasaba la validación exacta en board, aunque
  /// los datos tienen embarques con dos pasajeros o más. En el banco ruido, con noisy_contrast, la
  /// validez de los numéricos con un 3, 5, 10 y 20 % de ruido pasa de 97, 97, 97 y 94 % a 99 % en
  /// todos, sin cambiar la de STRIPS.
  bool value_chains = true;
  /// Si las trazas tienen ruido, la validación de la fusión exige que cada comparación numérica se
  /// cumpla en todos los ejemplos en que se conoce (rules::FusionOptions::min_comparison_support
  /// 1), como sin ruido (exact_without_noise). Es para usarla con smooth_values, que deja limpios
  /// los valores conocidos (no los átomos: la validación exacta de todo pierde efectos ciertos). En
  /// counters, con la validación de 0.95, pasaba (> (value ?0) 1) en decrement en lugar de
  /// (>= (value ?0) 1).
  bool exact_comparisons = true;
  /// Si las trazas tienen ruido, los acumuladores (que no se comparan ni pasan por el HRC:
  /// datasets::accumulators) se reconocen con el ruido: antes de las tablas, los que suben más de
  /// lo que explica el ruido de los valores y cuyas bajadas sí lo explica; y, después de los
  /// efectos, también los que solo tienen efectos increase. Con ruido, una alteración puede hacer
  /// que un acumulador parezca bajar: (total-fuel-used) se comparaba ((>= (total-fuel-used)
  /// (onboard ?1)) en board de zenotravel, que obliga a volar antes de subir al segundo pasajero)
  /// y pasaba por el HRC.
  bool noisy_accumulators = true;
  /// Si las trazas tienen ruido en los valores, el modelo nulo de la regresión de los fluentes sin
  /// argumentos (los que smooth_values no suaviza) tiene en cuenta el ruido
  /// (datasets::EffectOptions::noise).
  bool noisy_null_model = true;
  /// Si las trazas tienen ruido, en la tabla de cada acción, los valores de cada fluente se agrupan
  /// con HRC y pasan a la media de su cluster, y los atípicos, a desconocidos
  /// (datasets::smooth_fluents: el filtro numérico de PlanMiner-N). Va antes de buscar los efectos
  /// y las relaciones, que así se calculan con los valores suavizados. Salvo (total-cost) y los
  /// acumuladores (datasets::accumulators): sus valores no se repiten, y suavizarlos estropea la
  /// regresión de lo que suman (en barman, (increase (total-cost) 1) en todas las acciones). Si no,
  /// como el original.
  bool smooth_fluents = true;
  /// Si las trazas tienen ruido, la fusión refina los meta-estados como PlanMiner-N: fuera las
  /// condiciones con un soporte por debajo del 5 % de las aplicaciones, y en un conflicto con una
  /// diferencia de soportes dentro del 10 % de su media, fuera los dos valores
  /// (rules::FusionOptions::min_rule_support y conflict_band). Si no, como el original.
  bool refine_meta_states = true;
  /// A las reglas de cada acción se añade la de las cotas numéricas que se cumplen en sus estados
  /// de antes, se alcanzan en alguno y no son invariantes (rules::contrast_rule). Son
  /// precondiciones que INSLV no ve: datasets::numeric_relations descarta las relaciones constantes
  /// en las filas de la acción, como (> (onboard ?0) (zoom-limit ?0)) en fly-fast, porque
  /// pasárselas todas añade coincidencias. Las candidatas (datasets::candidate_relations) incluyen
  /// esas constantes y las de los fluentes que cambian en otras acciones. Sin noisy_contrast,
  /// solo sin ruido (plantraces::frame_changes es 0): con ruido sin suavizar, casi ninguna
  /// relación es un invariante (el ruido la contradice en algún estado), y pasan cotas que son
  /// coincidencias.
  bool contrast = true;
  /// La regla de contraste también con ruido, con el min_support de las comparaciones
  /// (exact_comparisons). Es para usarla con smooth_values y value_chains, que dejan limpios los
  /// valores conocidos: en zenotravel recupera (<= (onboard ?0) (zoom-limit ?0)) en fly-fast. La
  /// mejorada la usa.
  bool noisy_contrast = true;
  /// De la precondición fusionada se quita lo que implican las demás condiciones en los estados de
  /// las trazas (drop_implied): la inversa de una relación simétrica o un invariante del estado,
  /// que se cumplen siempre en los datos. Además de sobrar, las numéricas redundantes empeoran la
  /// heurística de los planificadores: (>= (onboard ?1) 1) en debark, implicada por (in ?0 ?1).
  bool drop_implied = true;
  /// El dominio declara :numeric-fluents si tiene funciones. El original no lo declara, y algunos
  /// planificadores (PyPlanner) rechazan el dominio.
  bool numeric_fluents = true;
  /// Las igualdades se escriben con =. El original escribe ==, que no es PDDL.
  bool equality = true;

  static Options original();
};

/// El aprendizaje de principio a fin: para cada acción, en orden, la tabla de ejemplos; los
/// ejemplos lógicos y sus reglas; los efectos y las relaciones numéricas, los ejemplos numéricos
/// (discretizados con el generador de INSLV) y sus reglas; y la fusión.
///
/// No usa el rand() global: el generador de INSLV da su secuencia y se recorre en el mismo orden
/// que el original, así que con Options::original() da los mismos ejemplos y reglas.
LearnedDomain learn(const std::vector<plantraces::Trace>& traces, const Options& options = {});

/// Como learn, pero el suavizado de los átomos (Options::smooth_atoms) cambia las trazas en lugar
/// de una copia: con muchas trazas, la copia puede no caber en memoria.
LearnedDomain learn(std::vector<plantraces::Trace>&& traces, const Options& options = {});

/// El dominio como lo escribe el PlanMiner original (el final de Old/src/bin/mainPlanMiner.cpp),
/// con el nombre en minúsculas:
///
/// - requisitos :strips y :typing (y, con options.numeric_fluents, :numeric-fluents si hay
///   funciones);
/// - los tipos agrupados por su padre ("object" si no tienen; un dominio sin tipos queda como
///   "object - object");
/// - los predicados y las funciones en el orden en que aparecen en los ejemplos de las acciones,
///   con los tipos de sus argumentos (las secciones salen aunque queden vacías: basta con que haya
///   relaciones o el instante);
/// - cada acción con sus parámetros y los tipos de la cabecera; en la precondición, los átomos
///   ciertos (los falsos se pierden) y las comparaciones (las igualdades, con == o, con
///   options.equality, con =); en el efecto, los átomos, los cambios
///   (increase si su valor es positivo, decrease si no), las asignaciones y los efectos de la
///   regresión. Nada que mencione el instante.
///
/// Dos diferencias con el original, por el formato de las trazas, no por el algoritmo: el original
/// busca la cabecera de cada predicado con su nombre en mayúsculas, como venían sus trazas (aquí,
/// con el nombre tal cual); y con una acción sin parámetros lee fuera de rango (aquí, "()").
std::string write(const std::string& name, const LearnedDomain& domain,
                  const Options& options = {});

}  // namespace pddl
