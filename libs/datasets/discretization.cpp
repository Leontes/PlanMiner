#include "datasets/discretization.hpp"

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <utility>

namespace datasets {

namespace {

// El código sigue al original paso a paso (Old/src/Datasets/Utils.cpp), con sus nombres entre
// paréntesis, para que dé los mismos números: el mismo orden de las operaciones, las mismas
// divisiones enteras y los mismos truncados.

constexpr std::size_t bins = 20;
constexpr std::size_t max_iterations = 100000;

struct Point {
  double x = 0.0;
  double y = 0.0;
};

enum class Orientation { collinear, clockwise, counterclockwise };

// La conversión de double a int de x86 (cvttsd2si): trunca, y fuera de rango da INT_MIN.
int truncate(double value) {
  if (!(value > -2147483649.0 && value < 2147483648.0)) {
    return INT_MIN;
  }
  return static_cast<int>(value);
}

// (enlaLinea)
bool on_segment(Point p, Point q, Point r) {
  return q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && q.y <= std::max(p.y, r.y) &&
         q.y >= std::min(p.y, r.y);
}

// (getOrientacion) La orientación se trunca a entero.
Orientation orientation(Point p, Point q, Point r) {
  const int value = truncate(((q.y - p.y) * (r.x - q.x)) - ((q.x - p.x) * (r.y - q.y)));
  if (value == 0) {
    return Orientation::collinear;
  }
  return value > 0 ? Orientation::clockwise : Orientation::counterclockwise;
}

// (intersectan)
bool intersect(Point p1, Point q1, Point p2, Point q2) {
  const Orientation o1 = orientation(p1, q1, p2);
  const Orientation o2 = orientation(p1, q1, q2);
  const Orientation o3 = orientation(p2, q2, p1);
  const Orientation o4 = orientation(p2, q2, q1);
  if (o1 != o2 && o3 != o4) {
    return true;
  }
  return (o1 == Orientation::collinear && on_segment(p1, p2, q1)) ||
         (o2 == Orientation::collinear && on_segment(p1, q2, q1)) ||
         (o3 == Orientation::collinear && on_segment(p2, p1, q2)) ||
         (o4 == Orientation::collinear && on_segment(p2, q1, q2));
}

// (distintos)
bool distinct(Point p, Point q) { return p.x != q.x || p.y != q.y; }

struct Clusters {
  double ch = 0.0;
  std::vector<double> centroids;
  std::vector<std::vector<double>> elements;
};

// (CH) El índice de Calinski-Harabasz, como en el original: SSB solo tiene el último cluster, y
// (N - k) / (k - 1) es una división entera.
double calinski_harabasz(const std::vector<double>& centroids,
                         const std::vector<std::vector<double>>& elements, double mean) {
  double ssw = 0.0;
  std::size_t n = 0;
  for (std::size_t k = 0; k < elements.size(); ++k) {
    for (const double element : elements[k]) {
      ssw += std::pow(element - centroids[k], 2);
    }
    n += elements[k].size();
  }
  double ssb = 0.0;
  for (std::size_t k = 0; k < elements.size(); ++k) {
    ssb = static_cast<double>(elements[k].size()) * std::pow(centroids[k] - mean, 2);
  }
  if (ssw == 0.0) {
    ssw = 1.0;
  }
  double ch = ssb / ssw;
  const std::size_t k = centroids.size();
  if (k > 1) {
    ch *= static_cast<double>((n - k) / (k - 1));
  } else {
    ch *= static_cast<double>(n - k);
  }
  return ch;
}

// (kmeans)
Clusters kmeans(std::size_t k, const std::vector<double>& values, const Random& random) {
  const auto n = static_cast<unsigned int>(values.size());
  std::vector<double> centroids(k, 0.0);
  std::vector<double> previous(k, 0.0);
  const unsigned int partition = n / static_cast<unsigned int>(k);

  double mean = 0.0;
  for (const double value : values) {
    mean += value;
  }
  mean /= static_cast<double>(values.size());

  for (std::size_t i = 0; i < k; ++i) {
    centroids[i] = values[(partition * i) + (partition / 2)];
  }

  std::vector<std::vector<double>> elements(k);
  std::size_t iterations = 0;
  do {
    for (std::vector<double>& cluster : elements) {
      cluster.clear();
    }
    previous = centroids;
    for (const double value : values) {
      double nearest = std::numeric_limits<double>::max();
      std::size_t cluster = 0;
      for (std::size_t c = 0; c < centroids.size(); ++c) {
        const double distance = std::pow(value - centroids[c], 2);
        if (distance < nearest) {
          cluster = c;
          nearest = distance;
        }
      }
      elements[cluster].push_back(value);
    }
    for (std::size_t i = 0; i < elements.size(); ++i) {
      centroids[i] = 0;
      for (const double element : elements[i]) {
        centroids[i] += element;
      }
      if (!elements[i].empty()) {
        centroids[i] /= static_cast<double>(elements[i].size());
      } else {
        const double drawn =
            static_cast<double>(random()) /
            static_cast<double>(static_cast<std::size_t>(RAND_MAX) / values.size());
        centroids[i] = values[std::min(static_cast<std::size_t>(drawn), values.size() - 1)];
      }
    }
  } while (centroids != previous && ++iterations < max_iterations);

  return {calinski_harabasz(centroids, elements, mean), centroids, elements};
}

// Una barra del histograma: el intervalo [from, to) y el porcentaje de los valores del cluster.
struct Bin {
  double from = 0.0;
  double to = 0.0;
  double percent = 0.0;
};

// (la segunda parte de discretizar) El trapecio de un cluster.
FuzzySet trapezoid(std::vector<double> elements) {
  std::sort(elements.begin(), elements.end());
  const double low = elements.front();
  const double high = elements.back();
  const double size = static_cast<double>(elements.size());
  const double width = (high - low) / 20.0;  // (particion)

  std::vector<Bin> histogram;  // (cortes)
  if (low == high) {
    histogram.push_back({low, high, 100.0});
  } else {
    double cut = low;
    std::size_t index = 0;
    for (std::size_t j = 0; j < bins; ++j) {
      cut += width;
      std::size_t count = 0;
      while (index < elements.size() && elements[index] < cut) {
        ++index;
        ++count;
      }
      histogram.push_back({cut - width, cut, 100.0 * (static_cast<double>(count) / size)});
    }
  }

  // La primera moda (maxindex). El original guarda el último porcentaje truncado a entero.
  std::size_t mode = 0;
  unsigned int last = 0;
  while (mode < histogram.size() && !(histogram[mode].percent < static_cast<double>(last))) {
    last = static_cast<unsigned int>(histogram[mode].percent);
    ++mode;
  }
  --mode;

  const Point a{histogram.front().from - (width / 500.0), 0.0};
  const Point d{histogram.back().to + (width / 100.0), 0.0};
  Point b = a;
  Point c = d;
  if (distinct(a, d)) {
    // (Bindex) Hacia la izquierda, mientras el segmento AB cruce alguna barra.
    std::size_t left = mode;
    bool done = false;
    while (!done) {
      b = {histogram[left].from, histogram[left].percent};
      done = true;
      for (std::size_t j = left; j > 0; --j) {
        const Point p{histogram[j].from, 0.0};
        const Point q{histogram[j].from, histogram[j].percent};
        if (intersect(a, b, p, q) && distinct(p, q)) {
          done = false;
          --left;
          break;
        }
      }
    }
    // (Cindex) Hacia la derecha, mientras el segmento CD cruce alguna barra.
    std::size_t right = mode;
    done = false;
    while (right < bins && !done) {
      c = {histogram[right].to, histogram[right].percent};
      done = true;
      for (std::size_t j = right; j < bins; ++j) {
        const Point p{histogram[j].from, 0.0};
        const Point q{histogram[j].from, histogram[j].percent};
        if (intersect(c, d, p, q) && distinct(p, q)) {
          done = false;
          ++right;
          break;
        }
      }
    }
  }
  return {a.x, b.x, c.x, d.x};
}

}  // namespace

std::vector<FuzzySet> discretize(const std::vector<double>& values, const Random& random) {
  std::vector<double> known;
  for (const double value : values) {
    if (!std::isnan(value)) {
      known.push_back(value);
    }
  }
  if (known.empty()) {
    return {};
  }

  // (discretizar) k = 1, 2, ... mientras mejore el índice.
  Clusters best;
  best.ch = -1.0;
  for (std::size_t k = 1;; ++k) {
    Clusters clusters = kmeans(k, known, random);
    if (!(best.ch < clusters.ch || best.ch == -1.0)) {
      break;
    }
    best = std::move(clusters);
  }

  std::vector<FuzzySet> sets;
  for (const std::vector<double>& elements : best.elements) {
    if (!elements.empty()) {
      sets.push_back(trapezoid(elements));
    }
  }
  return sets;
}

}  // namespace datasets
