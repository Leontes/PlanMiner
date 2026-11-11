// La discretización de los atributos numéricos: la partición difusa de sus valores que recibe el
// aprendizaje de reglas.
#pragma once

#include <functional>
#include <vector>

namespace datasets {

/// Una etiqueta difusa trapezoidal: [a, d] es su soporte y [b, c], su núcleo.
struct FuzzySet {
  double a = 0.0;
  double b = 0.0;
  double c = 0.0;
  double d = 0.0;

  bool operator==(const FuzzySet&) const = default;
};

/// Un generador de números aleatorios como rand(): un entero en [0, RAND_MAX].
using Random = std::function<int()>;

/// La partición difusa de los valores conocidos (no NaN) de un atributo numérico, como la calcula
/// el PlanMiner original (discretize en Old/src/Datasets/Utils.cpp), con sus fallos:
///
/// - k-means con k = 1, 2, ... mientras mejore el índice CH, que solo suma la separación del
///   último cluster (SSB = en vez de +=) y divide enteros. Los centroides empiezan en valores
///   repartidos por el orden de los datos, y un cluster que se queda vacío se resiembra con un
///   valor al azar (random).
/// - De cada cluster no vacío, en el orden de k-means (no por valor), un trapecio: el soporte, sus
///   valores con un pequeño margen; el núcleo, de un histograma de 20 barras, buscando la primera
///   moda y los puntos desde los que se ve el soporte sin cruzar ninguna barra. La orientación de
///   los puntos se trunca a entero, y la última barra no cuenta el valor máximo.
///
/// Lo que el original deja indefinido: si el histograma no baja nunca, la búsqueda de la moda lee
/// fuera de la tabla; aquí se para en la última barra. Si la orientación no cabe en un int, el
/// original tiene comportamiento indefinido; aquí vale INT_MIN, lo que da x86. Si el valor al
/// azar se sale de los datos (rand() casi igual a RAND_MAX), se toma el último. Y k-means para
/// tras 100000 iteraciones, aunque no haya convergido (el original no para).
///
/// Sin valores conocidos, la partición está vacía (el original lee fuera de rango).
std::vector<FuzzySet> discretize(const std::vector<double>& values, const Random& random);

/// Cómo se parten los clusters en la discretización HRC (hierarchical_discretize).
struct HierarchicalOptions {
  /// Un cluster se parte en dos si alpha (1 - s) + beta σ / n pasa de threshold: s, su silueta (con
  /// distancias al cuadrado); σ, la raíz de la suma de los cuadrados de las distancias de sus
  /// valores a su media; y n, los valores del conjunto que se partió.
  double threshold = 0.05;
  double alpha = 0.5;
  double beta = 0.5;
  /// Los valores se llevan a [0, 1] (con el mínimo y el máximo de la columna) antes de agruparlos,
  /// para que σ / n no dependa de las unidades. La rama ExperimentosRuido no lo hacía: con valores
  /// grandes, como el combustible de zenotravel, parte casi hasta un cluster por valor.
  bool normalize = false;
  /// Un cluster de un solo valor que habría que partir se queda. La rama lo descartaba, y su valor
  /// quedaba fuera de la partición.
  bool keep_singletons = false;
};

/// La discretización HRC (Hierarchical Recursive Clustering) de la rama ExperimentosRuido del
/// PlanMiner original: de arriba abajo, sin saber cuántos clusters hay. Los valores conocidos se
/// parten en dos con k-means (el de discretize, la mejor de cinco ejecuciones por el índice CH) y
/// cada cluster que no es lo bastante bueno (HierarchicalOptions) se vuelve a partir.
///
/// Cada cluster da una etiqueta exacta con sus valores, [mínimo, máximo], en orden: en la rama,
/// cada valor pasaba al centroide de su cluster y cada centroide era una etiqueta. Los valores de
/// los clusters descartados no caen en ninguna.
///
/// La rama solo la usó en una versión en que no hacía nada (k-means no calculaba la silueta, y no
/// salía ningún cluster). Esta es la de su último commit, con los clusters vacíos y la profundidad
/// acotados.
std::vector<FuzzySet> hierarchical_discretize(const std::vector<double>& values,
                                              const Random& random,
                                              const HierarchicalOptions& options = {});

}  // namespace datasets
