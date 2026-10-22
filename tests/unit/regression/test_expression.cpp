#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include "regression/expression.hpp"

namespace {

using regression::Expression;
using regression::Operation;

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();

// a * (b - 2)
Expression example() {
  return Expression::binary(
      Operation::multiply, Expression::variable(0),
      Expression::binary(Operation::subtract, Expression::variable(1), Expression::constant(2)));
}

TEST(Expression, SizeCountsEveryNode) {
  EXPECT_EQ(Expression::constant(3).size(), 1U);
  EXPECT_EQ(example().size(), 5U);
}

TEST(Expression, EvaluatesEveryExample) {
  const std::vector<std::vector<double>> variables{{1, 2, 3}, {4, 5, 6}};
  EXPECT_EQ(example().evaluate(variables, 3), (std::vector<double>{2, 6, 12}));
}

TEST(Expression, UnknownValuesAndDivisionByZeroAreNaN) {
  const std::vector<std::vector<double>> variables{{1, unknown}, {0, 2}};
  const std::vector<double> values =
      Expression::binary(Operation::divide, Expression::variable(0), Expression::variable(1))
          .evaluate(variables, 2);
  EXPECT_TRUE(std::isnan(values[0]));  // 1 / 0
  EXPECT_TRUE(std::isnan(values[1]));  // desconocido / 2
}

TEST(Expression, PrefixNotationWithNames) {
  EXPECT_EQ(example().to_string({"(fuel ?0)", "(capacity ?0)"}),
            "(* (fuel ?0) (- (capacity ?0) 2))");
  EXPECT_EQ(Expression::constant(0.5).to_string({}), "0.5");
}

TEST(Expression, EqualityIsStructural) {
  EXPECT_EQ(example(), example());
  const Expression swapped = Expression::binary(
      Operation::multiply,
      Expression::binary(Operation::subtract, Expression::variable(1), Expression::constant(2)),
      Expression::variable(0));
  EXPECT_FALSE(example() == swapped);
}

}  // namespace
