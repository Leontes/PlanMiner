// La API de inslv::learn.

#include <gtest/gtest.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "inslv/inslv.hpp"

namespace {

inslv::Variable boolean(const std::string& name) {
  return {name, {{"False", 0, 0, 0, 0}, {"True", 1, 1, 1, 1}}};
}

// 40 ejemplos: la clase es el valor de a; b no tiene relación con ella.
inslv::Dataset class_is_a() {
  inslv::Dataset data;
  data.attributes = {boolean("a"), boolean("b")};
  data.target = {"Class", {{"Negative", 0, 0, 0, 0}, {"Positive", 1, 1, 1, 1}}};
  data.values.assign(2, {});
  for (std::size_t e = 0; e < 40; ++e) {
    const std::size_t a = e % 2;
    data.values[0].push_back(static_cast<double>(a));
    data.values[1].push_back(static_cast<double>((e / 2) % 2));
    data.classes.push_back(a);
  }
  return data;
}

std::string describe(const std::vector<inslv::Rule>& rules) {
  std::ostringstream out;
  for (const inslv::Rule& rule : rules) {
    out << "clase " << rule.consequent << " (" << rule.covered << "):";
    for (const inslv::Condition& condition : rule.antecedent) {
      out << " atributo " << condition.attribute << " en {";
      for (std::size_t label : condition.labels) {
        out << ' ' << label;
      }
      out << " }";
    }
    out << '\n';
  }
  return out.str();
}

TEST(Learn, FindsTheAttributeThatDecidesTheClass) {
  const std::vector<inslv::Rule> rules = inslv::learn(class_is_a());

  ASSERT_EQ(rules.size(), 2U) << describe(rules);
  for (const inslv::Rule& rule : rules) {
    ASSERT_EQ(rule.antecedent.size(), 1U) << describe(rules);
    EXPECT_EQ(rule.antecedent[0].attribute, 0U) << describe(rules);
    EXPECT_EQ(rule.antecedent[0].labels, std::vector<std::size_t>{rule.consequent})
        << describe(rules);
    EXPECT_EQ(rule.covered, 20.0) << describe(rules);
  }
}

TEST(Learn, SameSeedSameRules) {
  const inslv::Dataset data = class_is_a();
  EXPECT_EQ(describe(inslv::learn(data)), describe(inslv::learn(data)));
}

TEST(Learn, UnknownValuesAreAccepted) {
  inslv::Dataset data = class_is_a();
  data.values[1][0] = std::numeric_limits<double>::quiet_NaN();
  data.values[1][7] = std::numeric_limits<double>::quiet_NaN();
  EXPECT_FALSE(inslv::learn(data).empty());
}

TEST(Learn, QuietUnlessVerbose) {
  testing::internal::CaptureStdout();
  inslv::learn(class_is_a());
  EXPECT_EQ(testing::internal::GetCapturedStdout(), "");

  inslv::Options verbose;
  verbose.verbose = true;
  testing::internal::CaptureStdout();
  inslv::learn(class_is_a(), verbose);
  EXPECT_NE(testing::internal::GetCapturedStdout(), "");
}

TEST(Learn, WritesTheReportWhenAsked) {
  const std::filesystem::path report =
      std::filesystem::temp_directory_path() / "inslv-test-report.res";
  std::filesystem::remove(report);

  inslv::Options options;
  options.report = report;
  inslv::learn(class_is_a(), options);

  std::ifstream in(report);
  std::string first_line;
  std::getline(in, first_line);
  EXPECT_EQ(first_line, "Experiment: " + report.string());
  std::filesystem::remove(report);
}

TEST(Learn, RejectsInconsistentData) {
  inslv::Dataset data = class_is_a();
  data.values.pop_back();
  EXPECT_THROW(inslv::learn(data), std::invalid_argument);

  data = class_is_a();
  data.values[1].pop_back();
  EXPECT_THROW(inslv::learn(data), std::invalid_argument);

  data = class_is_a();
  data.classes[3] = 2;
  EXPECT_THROW(inslv::learn(data), std::invalid_argument);

  data = class_is_a();
  data.attributes[0].labels.clear();
  EXPECT_THROW(inslv::learn(data), std::invalid_argument);

  data = class_is_a();
  data.classes.clear();
  data.values = {{}, {}};
  EXPECT_THROW(inslv::learn(data), std::invalid_argument);
}

}  // namespace
