// La regresión simbólica de PlanMiner: un A* sobre el espacio de expresiones aritméticas.
//
// Cada nodo es una expresión. Su coste es el del PlanMiner original, f = MAPE / 10 + 2 * tamaño,
// y es meta si su error porcentual medio (MAPE) con algún objetivo baja de max_error. Las mejoras
// sobre el original son opciones, para poder medir cada una por separado: Options::original()
// reproduce la búsqueda original y Options{} es la mejorada.
#pragma once

#include <chrono>
#include <cstddef>
#include <optional>
#include <vector>

#include "regression/expression.hpp"

namespace regression {

/// Un valor que se busca expresar.
struct Target {
  /// El valor en cada ejemplo; NaN, desconocido.
  std::vector<double> values;
  /// Una variable que no vale como expresión de este objetivo, ni ninguna expresión que no se
  /// distinga de ella con el criterio de aceptación (MAPE < max_error con sus valores). Por
  /// ejemplo, el propio fluente en una asignación: ni f := f ni f := f + 0.01 son un efecto.
  std::optional<std::size_t> trivial_variable;
};

/// Los datos: las variables y los objetivos, con el mismo número de ejemplos. Una expresión
/// explica el objetivo con el que menos error tiene.
struct Problem {
  /// variables[v][e]: el valor de la variable v en el ejemplo e; NaN, desconocido.
  std::vector<std::vector<double>> variables;
  std::vector<Target> targets;
};

/// Cómo crecen las expresiones.
enum class Successors {
  /// Por la derecha, E op X, como en el original: no puede expresar a * (b + c).
  append,
  /// En cualquier hoja L, (L op X) y, si op no es conmutativa, (X op L).
  tree,
};

/// Cuándo dos expresiones son la misma.
enum class Duplicates {
  /// Si se escriben igual, como en el original.
  text,
  /// Si dan los mismos valores en todos los ejemplos (equivalencia observacional): se queda la
  /// primera, que es la más corta.
  values,
};

struct Options {
  Successors successors = Successors::tree;
  Duplicates duplicates = Duplicates::values;
  /// Las constantes con las que se construyen las expresiones.
  std::vector<double> constants{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  /// Quita las variables que valen lo mismo en todos los ejemplos en que se conocen (por ejemplo,
  /// distance(c, c) = 0): no aportan nada que no dé una constante.
  bool drop_constant_variables = true;
  /// MAPE (en %) por debajo del cual una expresión es meta.
  double max_error = 2.0;
  /// Ejemplos que tiene que poder evaluar una expresión, como mínimo, para medir su error.
  std::size_t min_examples = 2;
  /// Ejemplos que tiene que poder evaluar una expresión por cada nodo (redondeando hacia arriba),
  /// si son más que min_examples. Con muy pocas filas, una expresión grande puede cuadrar por
  /// casualidad: con los problemas del catálogo de zenotravel y un 30 % de huecos, fly-fast
  /// aprendía (assign (total-fuel-used) (* (distance ?2 ?1) (+ (zoom-limit ?0) 2))) con 3 filas.
  /// Con 0.5, una de 5 nodos necesita 3 y (* (distance ?1 ?2) (fast-burn ?0)), de 3, 2. Con 0,
  /// como el original, solo min_examples.
  double min_examples_per_node = 0.5;
  /// Tamaño máximo de las expresiones (nodos); 0, sin límite. Con límite el espacio de búsqueda es
  /// finito: si se recorre entero sin llegar a los límites de seguridad, no hay expresión de ese
  /// tamaño.
  std::size_t max_size = 7;
  /// Límites de seguridad; 0, sin límite. Al llegar a uno no hay expresión. El de expansiones
  /// también acota la memoria (la lista de abiertos crece con cada expansión) y, a diferencia del
  /// de tiempo, da siempre el mismo resultado.
  std::size_t max_expansions = 50000;
  std::chrono::milliseconds time_limit{60000};
  /// Límite blando de memoria (MB de memoria residente del proceso), que se comprueba cada
  /// memory_check_interval expansiones; 0, sin límite. Si el proceso tiene un límite duro
  /// (resources::limit_process) y una reserva falla, la búsqueda también para.
  double max_memory_mb = 2048;
  std::size_t memory_check_interval = 1024;

  /// La búsqueda del PlanMiner original: crecimiento por la derecha, duplicados por texto,
  /// constantes de 0 a 10 y -1, sin quitar variables, sin límite de tamaño y 300 s. A diferencia
  /// del original, evalúa con todos los ejemplos (no con una muestra aleatoria), tiene el mismo
  /// límite de expansiones que la mejorada (sin él, agota la memoria antes que el tiempo) y, al
  /// llegar a un límite, no da expresión.
  static Options original();
};

struct Result {
  /// La expresión encontrada, si la hay.
  std::optional<Expression> expression;
  /// El objetivo que explica (índice en Problem::targets) y su MAPE.
  std::size_t target = 0;
  double error = 0.0;
  std::size_t expansions = 0;
  /// Si la búsqueda ha parado por un límite de seguridad: expansiones, tiempo o memoria.
  bool limit_reached = false;
};

/// Busca la expresión de menor coste que explica algún objetivo.
Result search(const Problem& problem, const Options& options = {});

/// El MAPE (en %) de los valores con el objetivo, como en el original: en los ejemplos con
/// |objetivo| < 1, el error absoluto; en el resto, el relativo. Solo cuentan los ejemplos en que
/// se conocen los dos. nullopt si son menos de min_examples.
std::optional<double> mape(const std::vector<double>& values, const std::vector<double>& target,
                           std::size_t min_examples);

}  // namespace regression
