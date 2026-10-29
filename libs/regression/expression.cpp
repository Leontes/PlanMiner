#include "regression/expression.hpp"

#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace regression {

struct Expression::Node {
  Operation operation = Operation::constant;
  double value = 0.0;
  std::size_t index = 0;
  std::size_t size = 1;
  // Solo en las operaciones binarias.
  std::unique_ptr<const Expression> left;
  std::unique_ptr<const Expression> right;
};

Expression::Expression(std::shared_ptr<const Node> node) : node_(std::move(node)) {}

Expression Expression::constant(double value) {
  auto node = std::make_shared<Node>();
  node->operation = Operation::constant;
  node->value = value;
  return Expression(std::move(node));
}

Expression Expression::variable(std::size_t index) {
  auto node = std::make_shared<Node>();
  node->operation = Operation::variable;
  node->index = index;
  return Expression(std::move(node));
}

Expression Expression::binary(Operation operation, Expression left, Expression right) {
  if (operation == Operation::constant || operation == Operation::variable) {
    throw std::invalid_argument("Expression::binary: la operación no es binaria");
  }
  auto node = std::make_shared<Node>();
  node->operation = operation;
  node->size = 1 + left.size() + right.size();
  node->left = std::make_unique<const Expression>(std::move(left));
  node->right = std::make_unique<const Expression>(std::move(right));
  return Expression(std::move(node));
}

Operation Expression::operation() const { return node_->operation; }

bool Expression::is_leaf() const {
  return node_->operation == Operation::constant || node_->operation == Operation::variable;
}

double Expression::value() const { return node_->value; }

std::size_t Expression::index() const { return node_->index; }

const Expression& Expression::left() const { return *node_->left; }

const Expression& Expression::right() const { return *node_->right; }

std::size_t Expression::size() const { return node_->size; }

std::vector<double> Expression::evaluate(const std::vector<std::vector<double>>& variables,
                                         std::size_t examples) const {
  switch (operation()) {
    case Operation::constant:
      return std::vector<double>(examples, value());
    case Operation::variable:
      return variables.at(index());
    default:
      break;
  }
  std::vector<double> result = left().evaluate(variables, examples);
  const std::vector<double> other = right().evaluate(variables, examples);
  for (std::size_t e = 0; e < examples; ++e) {
    switch (operation()) {
      case Operation::add:
        result[e] += other[e];
        break;
      case Operation::subtract:
        result[e] -= other[e];
        break;
      case Operation::multiply:
        result[e] *= other[e];
        break;
      default:
        result[e] =
            other[e] == 0.0 ? std::numeric_limits<double>::quiet_NaN() : result[e] / other[e];
        break;
    }
  }
  return result;
}

std::string Expression::to_string(const std::vector<std::string>& names) const {
  switch (operation()) {
    case Operation::constant: {
      std::ostringstream text;
      text << value();
      return text.str();
    }
    case Operation::variable:
      return names.at(index());
    case Operation::add:
      return "(+ " + left().to_string(names) + " " + right().to_string(names) + ")";
    case Operation::subtract:
      return "(- " + left().to_string(names) + " " + right().to_string(names) + ")";
    case Operation::multiply:
      return "(* " + left().to_string(names) + " " + right().to_string(names) + ")";
    case Operation::divide:
      return "(/ " + left().to_string(names) + " " + right().to_string(names) + ")";
  }
  return "";
}

bool Expression::operator==(const Expression& other) const {
  if (node_ == other.node_) {
    return true;
  }
  if (operation() != other.operation()) {
    return false;
  }
  switch (operation()) {
    case Operation::constant:
      return value() == other.value();
    case Operation::variable:
      return index() == other.index();
    default:
      return left() == other.left() && right() == other.right();
  }
}

Expression fold_constants(const Expression& expression) {
  if (expression.is_leaf()) {
    return expression;
  }
  Expression left = fold_constants(expression.left());
  Expression right = fold_constants(expression.right());
  if (left.operation() == Operation::constant && right.operation() == Operation::constant) {
    const double value =
        Expression::binary(expression.operation(), left, right).evaluate({}, 1).front();
    if (std::isfinite(value)) {
      return Expression::constant(value);
    }
  }
  return Expression::binary(expression.operation(), std::move(left), std::move(right));
}

}  // namespace regression
