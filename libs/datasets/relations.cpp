#include "datasets/relations.hpp"

#include <cmath>
#include <cstddef>
#include <limits>
#include <utility>

namespace datasets {

namespace {

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();
constexpr Comparison comparisons[] = {Comparison::equal, Comparison::less, Comparison::greater};

bool holds(Comparison comparison, double left, double right) {
  switch (comparison) {
    case Comparison::equal:
      return left == right;
    case Comparison::less:
      return left < right;
    case Comparison::greater:
      return left > right;
  }
  return false;
}

// Si la relación aporta algo: vectorInutil del original, que compara con la primera fila aunque
// sea desconocida.
bool varies(const std::vector<double>& column) {
  for (std::size_t row = 1; row < column.size(); ++row) {
    if (!std::isnan(column[row]) && !(column[row] == column[0])) {
      return true;
    }
  }
  return false;
}

}  // namespace

std::string to_string(const Relation& relation) {
  const char* comparison = "=";
  switch (relation.comparison) {
    case Comparison::equal:
      break;
    case Comparison::less:
      comparison = "<";
      break;
    case Comparison::greater:
      comparison = ">";
      break;
  }
  const std::string right = plantraces::to_string(relation.right);
  return std::string{"("} + comparison + " " + plantraces::to_string(relation.left) + " " +
         (relation.right_kind == OperandKind::fluent ? right : "(- " + right + " " + right + "')") +
         ")";
}

std::vector<Relation> numeric_relations(const Table& table) {
  const std::size_t n = table.transitions;
  std::vector<std::size_t> fluents;
  for (std::size_t a = 0; a < table.attributes.size(); ++a) {
    if (table.attributes[a].kind == AttributeKind::fluent) {
      fluents.push_back(a);
    }
  }

  // Lo que baja cada fluente que cambia en alguna aplicación, en el orden de la tabla.
  std::vector<std::pair<std::size_t, std::vector<double>>> falls;
  for (const std::size_t fluent : fluents) {
    const std::vector<double>& column = table.columns[fluent];
    std::vector<double> fall(n, unknown);
    bool changes = false;
    for (std::size_t i = 0; i < n; ++i) {
      if (!std::isnan(column[i]) && !std::isnan(column[n + i])) {
        fall[i] = column[i] - column[n + i];
        changes = changes || fall[i] != 0.0;
      }
    }
    if (changes) {
      falls.emplace_back(fluent, std::move(fall));
    }
  }

  std::vector<Relation> relations;
  const auto add = [&](Comparison comparison, std::size_t left, OperandKind kind, std::size_t right,
                       const auto& right_value) {
    Relation relation{comparison, table.attributes[left].term, kind, table.attributes[right].term,
                      std::vector<double>(table.rows(), unknown)};
    const std::vector<double>& values = table.columns[left];
    for (std::size_t row = 0; row < table.rows(); ++row) {
      const double other = right_value(row);
      if (!std::isnan(values[row]) && !std::isnan(other)) {
        relation.column[row] = holds(comparison, values[row], other) ? 1.0 : 0.0;
      }
    }
    if (varies(relation.column)) {
      relations.push_back(std::move(relation));
    }
  };

  for (std::size_t f = 0; f < fluents.size(); ++f) {
    const std::size_t left = fluents[f];
    bool changes = false;
    for (const auto& [fluent, fall] : falls) {
      changes = changes || fluent == left;
    }
    if (!changes) {
      continue;
    }
    for (std::size_t g = f + 1; g < fluents.size(); ++g) {
      const std::vector<double>& right = table.columns[fluents[g]];
      for (const Comparison comparison : comparisons) {
        add(comparison, left, OperandKind::fluent, fluents[g],
            [&right](std::size_t row) { return right[row]; });
      }
    }
    for (const auto& [fluent, fall] : falls) {
      if (fluent == left) {
        continue;
      }
      for (const Comparison comparison : comparisons) {
        add(comparison, left, OperandKind::fall, fluent,
            [&fall, n](std::size_t row) { return fall[row % n]; });
      }
    }
  }
  return relations;
}

}  // namespace datasets
