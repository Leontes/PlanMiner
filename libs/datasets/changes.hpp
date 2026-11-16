// Si un valor cambia en las aplicaciones de una acción: el criterio que comparten los efectos y las
// relaciones numéricas.
#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

#include "plantraces/noise.hpp"

namespace datasets::detail {

// Tolerancia relativa para decidir que un valor no ha cambiado.
constexpr double tolerance = 1e-6;

inline bool same(double before, double after) {
  return std::abs(after - before) <= tolerance * std::max(1.0, std::abs(before));
}

struct Unchanged {
  /// Las aplicaciones en que se conoce el valor antes y después.
  std::size_t known = 0;
  /// Las de esas en que no cambia.
  std::size_t unchanged = 0;

  /// Si cambia en alguna aplicación; con el modelo nulo, en más de una proporción 1 -
  /// max_unchanged de las conocidas y, con ruido q (la probabilidad de que dos observaciones del
  /// mismo valor difieran), en más de las que explica una binomial con alpha 1e-3.
  bool changes(bool null_model, double max_unchanged, double noise = 0.0) const {
    if (unchanged == known || !null_model) {
      return unchanged != known;
    }
    return static_cast<double>(unchanged) < max_unchanged * static_cast<double>(known) &&
           !(noise > 0.0 && known - unchanged <= plantraces::binomial_bound(known, noise, 1e-3));
  }
};

/// En cuántas aplicaciones se conoce el valor de una columna de la tabla (primero las filas de
/// antes y detrás las de después) y en cuántas de ellas no cambia.
inline Unchanged count_unchanged(const std::vector<double>& column, std::size_t transitions) {
  Unchanged count;
  for (std::size_t i = 0; i < transitions; ++i) {
    const double before = column[i];
    const double after = column[transitions + i];
    if (!std::isnan(before) && !std::isnan(after)) {
      ++count.known;
      count.unchanged += same(before, after) ? 1U : 0U;
    }
  }
  return count;
}

}  // namespace datasets::detail
