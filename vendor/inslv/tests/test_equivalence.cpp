// Equivalencia con la implementación original.
//
// Cada caso de cases/ es una llamada a inslv() del PlanMiner original: la entrada (variables,
// etiquetas y ejemplos) y las reglas que devolvió, ya interpretadas por PlanMiner. learn() tiene
// que dar las mismas reglas con la misma interpretación.
//
// Los casos se generan con el PlanMiner original (Old/ en el repositorio de PlanMiner, test
// oracle.inslv_cases).

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "inslv/inslv.hpp"

namespace {

// Una regla como la daba el PlanMiner original: los ejemplos cubiertos, con signo negativo si la
// clase es PreState, y los pares (atributo, valor).
struct LegacyRule {
  int covered = 0;
  std::vector<std::pair<std::string, std::string>> conditions;

  bool operator==(const LegacyRule& other) const {
    return covered == other.covered && conditions == other.conditions;
  }
};

std::ostream& operator<<(std::ostream& out, const LegacyRule& rule) {
  out << "rule " << rule.covered << ":";
  for (const auto& [attribute, value] : rule.conditions) {
    out << " [" << attribute << " = " << value << "]";
  }
  return out;
}

struct Case {
  inslv::Dataset data;
  std::vector<LegacyRule> rules;
};

// El resto de la línea tras el último campo leído, sin el espacio que lo separa.
std::string rest_of_line(std::istringstream& line) {
  std::string rest;
  std::getline(line >> std::ws, rest);
  return rest;
}

double parse_value(const std::string& token) {
  return token == "?" ? std::numeric_limits<double>::quiet_NaN() : std::stod(token);
}

// Lee la siguiente línea que no es un comentario y comprueba su palabra clave.
std::istringstream next_line(std::ifstream& in, const std::string& keyword) {
  std::string text;
  while (std::getline(in, text)) {
    if (!text.empty() && text.front() != '#') {
      std::istringstream line(text);
      std::string word;
      line >> word;
      if (word != keyword) {
        throw std::runtime_error("se esperaba '" + keyword + "' y hay: " + text);
      }
      return line;
    }
  }
  throw std::runtime_error("fin de fichero; se esperaba '" + keyword + "'");
}

inslv::Variable read_variable(std::ifstream& in, const std::string& keyword) {
  std::istringstream line = next_line(in, keyword);
  std::size_t labels = 0;
  line >> labels;
  inslv::Variable variable;
  variable.name = rest_of_line(line);
  for (std::size_t i = 0; i < labels; ++i) {
    std::istringstream label_line = next_line(in, "label");
    std::string a, b, c, d;
    inslv::FuzzySet set;
    label_line >> a >> b >> c >> d >> set.label;
    set.a = parse_value(a);
    set.b = parse_value(b);
    set.c = parse_value(c);
    set.d = parse_value(d);
    variable.labels.push_back(set);
  }
  return variable;
}

Case read_case(const std::filesystem::path& file) {
  std::ifstream in(file);
  Case result;

  std::size_t attributes = 0;
  next_line(in, "attributes") >> attributes;
  for (std::size_t i = 0; i < attributes; ++i) {
    result.data.attributes.push_back(read_variable(in, "attribute"));
  }
  result.data.target = read_variable(in, "target");

  std::size_t examples = 0;
  next_line(in, "examples") >> examples;
  result.data.values.assign(attributes, std::vector<double>(examples));
  result.data.classes.resize(examples);
  for (std::size_t e = 0; e < examples; ++e) {
    std::string text;
    std::getline(in, text);
    std::istringstream line(text);
    for (std::size_t i = 0; i < attributes; ++i) {
      std::string token;
      line >> token;
      result.data.values[i][e] = parse_value(token);
    }
    line >> result.data.classes[e];
  }

  std::size_t rules = 0;
  next_line(in, "rules") >> rules;
  for (std::size_t r = 0; r < rules; ++r) {
    std::istringstream line = next_line(in, "rule");
    LegacyRule rule;
    std::size_t conditions = 0;
    line >> rule.covered >> conditions;
    for (std::size_t c = 0; c < conditions; ++c) {
      std::istringstream condition = next_line(in, "condition");
      std::string value;
      condition >> value;
      rule.conditions.emplace_back(rest_of_line(condition), value);
    }
    result.rules.push_back(rule);
  }
  return result;
}

// La interpretación de PlanMiner (ruleset::SplitAntecedent y ruleset::toPDDL del original): solo
// cuentan los atributos de hasta dos etiquetas; el valor es el nombre de la etiqueta si es True o
// False y, si no, su centro truncado a entero. La clase va en el signo.
LegacyRule interpret(const inslv::Rule& rule, const inslv::Dataset& data) {
  LegacyRule result;
  const bool post_state = data.target.labels[rule.consequent].label == "PostState";
  result.covered = post_state ? static_cast<int>(rule.covered) : static_cast<int>(0 - rule.covered);
  for (const inslv::Condition& condition : rule.antecedent) {
    const inslv::Variable& attribute = data.attributes[condition.attribute];
    if (attribute.labels.size() > 2) {
      continue;
    }
    for (std::size_t label : condition.labels) {
      const inslv::FuzzySet& set = attribute.labels[label];
      if (set.label == "True" || set.label == "False") {
        result.conditions.emplace_back(attribute.name, set.label);
      } else {
        result.conditions.emplace_back(attribute.name,
                                       std::to_string(static_cast<int>((set.b + set.c) / 2.0)));
      }
    }
  }
  return result;
}

std::vector<std::filesystem::path> case_files() {
  std::vector<std::filesystem::path> files;
  for (const auto& entry : std::filesystem::directory_iterator(INSLV_CASES_DIR)) {
    if (entry.path().extension() == ".case") {
      files.push_back(entry.path());
    }
  }
  std::sort(files.begin(), files.end());
  return files;
}

class Equivalence : public testing::TestWithParam<std::filesystem::path> {};

TEST_P(Equivalence, SameRulesAsTheOriginal) {
  const Case expected = read_case(GetParam());

  std::vector<LegacyRule> learned;
  for (const inslv::Rule& rule : inslv::learn(expected.data)) {
    learned.push_back(interpret(rule, expected.data));
  }

  ASSERT_EQ(learned.size(), expected.rules.size());
  for (std::size_t r = 0; r < learned.size(); ++r) {
    EXPECT_EQ(learned[r], expected.rules[r]) << "regla " << r;
  }
}

INSTANTIATE_TEST_SUITE_P(Cases, Equivalence, testing::ValuesIn(case_files()),
                         [](const testing::TestParamInfo<std::filesystem::path>& file) {
                           std::string name = file.param.stem().string();
                           std::replace(name.begin(), name.end(), '-', '_');
                           return name;
                         });

}  // namespace
