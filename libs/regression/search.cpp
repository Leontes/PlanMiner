#include "regression/search.hpp"

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <new>
#include <queue>
#include <set>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>

#include "resources/memory.hpp"

namespace regression {

namespace {

// El error de una expresión que no se puede medir, como en el original.
constexpr double unmeasurable = 99999999.0;

constexpr Operation operations[] = {Operation::add, Operation::multiply, Operation::subtract,
                                    Operation::divide};

bool commutative(Operation operation) {
  return operation == Operation::add || operation == Operation::multiply;
}

struct Node {
  double priority = 0.0;
  double error = 0.0;
  std::size_t order = 0;
  Expression expression;
  std::size_t target = 0;
  bool goal = false;
};

// Orden de la lista de abiertos: menor coste, después menor error y después la más antigua.
struct Later {
  bool operator()(const Node& a, const Node& b) const {
    return std::tie(a.priority, a.error, a.order) > std::tie(b.priority, b.error, b.order);
  }
};

// Huella de los valores de una expresión, redondeados a 1e-6, para la equivalencia observacional.
std::uint64_t fingerprint(const std::vector<double>& values) {
  std::uint64_t hash = 14695981039346656037ULL;  // FNV-1a
  for (const double value : values) {
    std::uint64_t bits = 0x7ff8000000000001ULL;  // NaN
    if (!std::isnan(value)) {
      const double rounded = std::round(value * 1e6) / 1e6;
      bits = std::bit_cast<std::uint64_t>(rounded == 0.0 ? 0.0 : rounded);  // sin -0
    }
    for (int byte = 0; byte < 8; ++byte) {
      hash ^= (bits >> (8 * byte)) & 0xffU;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}

class Search {
 public:
  Search(const Problem& problem, const Options& options)
      : problem_(problem), options_(options), start_(std::chrono::steady_clock::now()) {
    examples_ = problem.targets.empty() ? 0 : problem.targets.front().values.size();
    for (const double constant : options.constants) {
      terminals_.push_back(Expression::constant(constant));
    }
    for (std::size_t v = 0; v < problem.variables.size(); ++v) {
      names_.push_back("v" + std::to_string(v));
      if (!options.drop_constant_variables || !constant_variable(problem.variables[v])) {
        terminals_.push_back(Expression::variable(v));
      }
    }
  }

  Result run() {
    Result result;
    try {
      explore(result);
    } catch (const std::bad_alloc&) {
      // El límite duro del proceso: la lista de abiertos se libera al salir.
      result.expression.reset();
      result.limit_reached = true;
    }
    return result;
  }

 private:
  void explore(Result& result) {
    for (const Expression& terminal : terminals_) {
      push(terminal);
    }
    while (!open_.empty()) {
      Node node = open_.top();
      open_.pop();
      if (node.goal) {
        result.expression = node.expression;
        result.target = node.target;
        result.error = node.error;
        return;
      }
      if (limit_reached(result.expansions)) {
        result.limit_reached = true;
        return;
      }
      ++result.expansions;
      if (options_.max_size == 0 || node.expression.size() + 2 <= options_.max_size) {
        for (Expression& child : successors(node.expression)) {
          push(std::move(child));
        }
      }
    }
  }

  static bool constant_variable(const std::vector<double>& values) {
    std::set<double> known;
    for (const double value : values) {
      if (!std::isnan(value)) {
        known.insert(value);
      }
    }
    return known.size() <= 1;
  }

  bool limit_reached(std::size_t expansions) const {
    if (options_.max_expansions != 0 && expansions >= options_.max_expansions) {
      return true;
    }
    if (options_.max_memory_mb > 0 && options_.memory_check_interval > 0 &&
        expansions % options_.memory_check_interval == 0) {
      const std::optional<double> resident = resources::resident_memory_mb();
      if (resident && *resident > options_.max_memory_mb) {
        return true;
      }
    }
    return options_.time_limit.count() > 0 &&
           std::chrono::steady_clock::now() - start_ >= options_.time_limit;
  }

  std::vector<Expression> successors(const Expression& expression) const {
    if (options_.successors == Successors::tree) {
      return grow(expression);
    }
    // E op X, en el orden del original: las constantes y después las variables.
    std::vector<Expression> children;
    for (const Operation operation : operations) {
      // Como en el original: nada de sumar o restar 0, ni de multiplicar o dividir por 0 o 1.
      const double smallest =
          operation == Operation::add || operation == Operation::subtract ? 1.0 : 2.0;
      for (const Expression& terminal : terminals_) {
        if (terminal.operation() == Operation::constant && terminal.value() < smallest) {
          continue;
        }
        children.push_back(Expression::binary(operation, expression, terminal));
      }
    }
    return children;
  }

  // Cada hoja L se sustituye por (L op X) y, si op no es conmutativa, también por (X op L).
  std::vector<Expression> grow(const Expression& expression) const {
    std::vector<Expression> children;
    if (expression.is_leaf()) {
      for (const Operation operation : operations) {
        for (const Expression& terminal : terminals_) {
          children.push_back(Expression::binary(operation, expression, terminal));
          if (!commutative(operation)) {
            children.push_back(Expression::binary(operation, terminal, expression));
          }
        }
      }
      return children;
    }
    for (Expression& left : grow(expression.left())) {
      children.push_back(Expression::binary(expression.operation(), left, expression.right()));
    }
    for (Expression& right : grow(expression.right())) {
      children.push_back(Expression::binary(expression.operation(), expression.left(), right));
    }
    return children;
  }

  void push(Expression expression) {
    const std::vector<double> values = expression.evaluate(problem_.variables, examples_);
    const bool fresh = options_.duplicates == Duplicates::values
                           ? seen_values_.insert(fingerprint(values)).second
                           : seen_text_.insert(expression.to_string(names_)).second;
    if (!fresh) {
      return;
    }

    Node node{0.0, unmeasurable, order_++, std::move(expression), 0, false};
    for (std::size_t t = 0; t < problem_.targets.size(); ++t) {
      const Target& target = problem_.targets[t];
      if (target.trivial_variable) {
        const std::optional<double> distance =
            mape(values, problem_.variables[*target.trivial_variable], options_.min_examples);
        if (distance && *distance < options_.max_error) {
          continue;  // no se distingue de la variable trivial
        }
      }
      const std::size_t needed = std::max(
          options_.min_examples,
          static_cast<std::size_t>(std::ceil(options_.min_examples_per_node *
                                             static_cast<double>(node.expression.size()))));
      const std::optional<double> error = mape(values, target.values, needed);
      if (error && *error < node.error) {
        node.error = *error;
        node.target = t;
      }
    }
    node.goal = node.error < options_.max_error;
    node.priority = node.error / 10.0 + 2.0 * static_cast<double>(node.expression.size());
    open_.push(std::move(node));
  }

  const Problem& problem_;
  const Options& options_;
  std::chrono::steady_clock::time_point start_;
  std::size_t examples_ = 0;
  std::vector<std::string> names_;
  std::vector<Expression> terminals_;
  std::priority_queue<Node, std::vector<Node>, Later> open_;
  std::unordered_set<std::uint64_t> seen_values_;
  std::unordered_set<std::string> seen_text_;
  std::size_t order_ = 0;
};

}  // namespace

Options Options::original() {
  Options options;
  options.successors = Successors::append;
  options.duplicates = Duplicates::text;
  options.constants = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -1};
  options.drop_constant_variables = false;
  options.min_examples = 1;
  options.min_examples_per_node = 0.0;
  options.max_size = 0;
  options.time_limit = std::chrono::milliseconds{300000};
  return options;
}

std::optional<double> mape(const std::vector<double>& values, const std::vector<double>& target,
                           std::size_t min_examples) {
  double sum = 0.0;
  std::size_t count = 0;
  for (std::size_t e = 0; e < values.size() && e < target.size(); ++e) {
    if (std::isnan(values[e]) || std::isnan(target[e])) {
      continue;
    }
    sum += std::abs(target[e]) < 1.0 ? std::abs(target[e] - values[e])
                                     : std::abs((target[e] - values[e]) / target[e]);
    ++count;
  }
  if (count == 0 || count < min_examples) {
    return std::nullopt;
  }
  return sum / static_cast<double>(count) * 100.0;
}

Result search(const Problem& problem, const Options& options) {
  return Search(problem, options).run();
}

}  // namespace regression
