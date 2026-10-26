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

}  // namespace datasets
