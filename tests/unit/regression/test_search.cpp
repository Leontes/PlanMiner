#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <limits>
#include <string>
#include <vector>

#include "regression/search.hpp"

namespace {

using regression::Expression;
using regression::Options;
using regression::Problem;
using regression::Result;

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();

// Tres variables con valores sin relaciones sencillas entre ellas.
const std::vector<std::vector<double>> abc{
    {3, 7, 2, 11, 5, 13}, {4, 1, 9, 6, 8, 2}, {5, 3, 7, 2, 11, 6}};

std::vector<double> values(const Expression& expression,
                           const std::vector<std::vector<double>>& variables) {
  return expression.evaluate(variables, variables.front().size());
}

// El problema de expresar f(a, b, c) con a, b y c.
template <typename F>
Problem problem(F f) {
  Problem result{abc, {{{}, std::nullopt}}};
  for (std::size_t e = 0; e < abc[0].size(); ++e) {
    result.targets[0].values.push_back(f(abc[0][e], abc[1][e], abc[2][e]));
  }
  return result;
}

std::string text(const Result& result) {
  return result.expression ? result.expression->to_string({"a", "b", "c"}) : "-";
}

TEST(Mape, AbsoluteErrorForSmallTargetsAndRelativeForTheRest) {
  // |0.5 - 0.4| = 0.1 y |10 - 9| / 10 = 0.1: de media, el 10 %.
  EXPECT_NEAR(*regression::mape({0.4, 9}, {0.5, 10}, 1), 10.0, 1e-9);
}

TEST(Mape, OnlyCountsExamplesWithBothValues) {
  EXPECT_NEAR(*regression::mape({1, unknown, 3}, {1, 5, unknown}, 1), 0.0, 1e-12);
  EXPECT_EQ(regression::mape({1, unknown, 3}, {1, 5, unknown}, 2), std::nullopt);
}

TEST(Search, FindsAProduct) {
  const Result result =
      regression::search(problem([](double a, double b, double) { return a * b; }));
  EXPECT_EQ(text(result), "(* a b)");
  EXPECT_LT(result.error, 1e-9);
}

TEST(Search, FindsAnExpressionWhoseRightOperandIsNotALeaf) {
  // a / (b + c): con crecimiento por la derecha, como en el original, no se puede escribir.
  const Problem divide = problem([](double a, double b, double c) { return a / (b + c); });
  const Result tree = regression::search(divide);
  ASSERT_TRUE(tree.expression);
  EXPECT_EQ(values(*tree.expression, abc), divide.targets[0].values);

  Options append;
  append.successors = regression::Successors::append;
  EXPECT_FALSE(regression::search(divide, append).expression);
}

TEST(Search, ExplainsTheTargetWithTheLeastError) {
  Problem two = problem([](double a, double, double) { return a + 100; });
  two.targets.push_back(
      {values(Expression::binary(regression::Operation::multiply, Expression::variable(1),
                                 Expression::variable(2)),
              abc),
       std::nullopt});
  const Result result = regression::search(two);
  EXPECT_EQ(result.target, 1U);
  ASSERT_TRUE(result.expression);
  EXPECT_EQ(values(*result.expression, abc), two.targets[1].values);  // b * c o c * b
}

TEST(Search, NothingIndistinguishableFromATrivialVariableExplainsItsTarget) {
  // El objetivo es a, con a trivial: valdría d / 2 con d = 2a, o a + 0.01, pero no se distinguen
  // de a con el criterio de aceptación.
  Problem same = problem([](double a, double, double) { return a; });
  std::vector<double> d;
  for (const double a : abc[0]) {
    d.push_back(2 * a);
  }
  same.variables.push_back(d);
  same.targets[0].trivial_variable = 0;
  const Result result = regression::search(same);
  EXPECT_FALSE(result.expression) << result.expression->to_string({"a", "b", "c", "d"});
}

TEST(Search, ATrivialVariableCanBePartOfAnExpression) {
  // 3a sí se distingue de a.
  Problem triple = problem([](double a, double, double) { return 3 * a; });
  triple.targets[0].trivial_variable = 0;
  const Result result = regression::search(triple);
  ASSERT_TRUE(result.expression);
  EXPECT_EQ(values(*result.expression, abc), triple.targets[0].values);
}

TEST(Search, DuplicatesByValuesExpandFewerNodes) {
  const Problem product = problem([](double a, double b, double c) { return a * b * c + a; });
  Options by_text;
  by_text.duplicates = regression::Duplicates::text;
  const Result values_result = regression::search(product);
  const Result text_result = regression::search(product, by_text);
  ASSERT_TRUE(values_result.expression);
  ASSERT_TRUE(text_result.expression);
  EXPECT_LT(values_result.expansions, text_result.expansions);
}

TEST(Search, ConstantVariablesAreDroppedByDefault) {
  // z = 0 en todos los ejemplos: sin quitarla, -b se escribe como (z - b).
  Problem negative = problem([](double, double b, double) { return -b; });
  negative.variables.push_back(std::vector<double>(abc[0].size(), 0.0));
  const std::vector<std::string> names{"a", "b", "c", "z"};

  Options keep;
  keep.drop_constant_variables = false;
  EXPECT_EQ(regression::search(negative, keep).expression->to_string(names), "(- z b)");

  const Result dropped = regression::search(negative);
  ASSERT_TRUE(dropped.expression);
  EXPECT_EQ(dropped.expression->to_string(names).find('z'), std::string::npos);
}

TEST(Search, NeedsEnoughExamples) {
  Problem one = problem([](double a, double b, double) { return a * b; });
  for (std::size_t e = 1; e < abc[0].size(); ++e) {
    one.targets[0].values[e] = unknown;
  }
  EXPECT_FALSE(regression::search(one).expression);  // con un solo ejemplo, cualquier cosa vale
}

TEST(Search, StopsAtTheExpansionLimitWithoutAnExpression) {
  Options limited;
  limited.max_expansions = 10;
  const Result result = regression::search(
      problem([](double a, double b, double c) { return a * a * a - b * c / 7; }), limited);
  EXPECT_FALSE(result.expression);
  EXPECT_TRUE(result.limit_reached);
  EXPECT_EQ(result.expansions, 10U);
}

TEST(Search, WithASizeLimitTheSearchIsFinite) {
  // Con dos variables, dos constantes y expresiones de hasta 5 nodos, el espacio se recorre entero
  // sin llegar a los límites de seguridad: no hay expresión.
  Problem small{{abc[0], abc[1]}, {{{}, std::nullopt}}};
  for (std::size_t e = 0; e < abc[0].size(); ++e) {
    small.targets[0].values.push_back(abc[0][e] * abc[0][e] * abc[0][e] - abc[1][e] / 7);
  }
  Options options;
  options.constants = {1, 2};
  options.max_size = 5;
  const Result result = regression::search(small, options);
  EXPECT_FALSE(result.expression);
  EXPECT_FALSE(result.limit_reached);
}

TEST(Search, IsDeterministic) {
  const Problem sum = problem([](double a, double b, double c) { return a + b * c; });
  const Result first = regression::search(sum);
  const Result second = regression::search(sum);
  EXPECT_EQ(text(first), text(second));
  EXPECT_EQ(first.expansions, second.expansions);
}

TEST(Search, TheOriginalOptions) {
  const Options original = Options::original();
  EXPECT_EQ(original.successors, regression::Successors::append);
  EXPECT_EQ(original.duplicates, regression::Duplicates::text);
  EXPECT_EQ(original.max_size, 0U);
  EXPECT_FALSE(original.drop_constant_variables);
  EXPECT_EQ(
      text(regression::search(problem([](double a, double b, double) { return a * b; }), original)),
      "(* a b)");
}

}  // namespace
