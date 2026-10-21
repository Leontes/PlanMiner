// La API de inslv.hpp sobre la implementación original (VectorVar, example_set y Aprender).

#include "inslv/inslv.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <vector>

#include "RuleCreator.hpp"

namespace inslv {

namespace {

// El valor que marca un dato desconocido en la implementación (example_set::MISSING y
// fuzzy_t::MISSING, privados): se adapta por completo a todas las etiquetas.
constexpr double missing_value = -999999999;

void check_labels(const Variable& variable) {
  if (variable.labels.empty()) {
    throw std::invalid_argument("inslv: la variable '" + variable.name + "' no tiene etiquetas");
  }
}

void check(const Dataset& data) {
  if (data.attributes.empty()) {
    throw std::invalid_argument("inslv: no hay atributos");
  }
  for (const Variable& attribute : data.attributes) {
    check_labels(attribute);
  }
  check_labels(data.target);

  if (data.values.size() != data.attributes.size()) {
    throw std::invalid_argument("inslv: hay " + std::to_string(data.attributes.size()) +
                                " atributos y " + std::to_string(data.values.size()) +
                                " columnas de valores");
  }
  const std::size_t examples = data.classes.size();
  if (examples == 0) {
    throw std::invalid_argument("inslv: no hay ejemplos");
  }
  for (std::size_t i = 0; i < data.values.size(); ++i) {
    if (data.values[i].size() != examples) {
      throw std::invalid_argument("inslv: el atributo '" + data.attributes[i].name + "' tiene " +
                                  std::to_string(data.values[i].size()) + " valores y hay " +
                                  std::to_string(examples) + " ejemplos");
    }
  }
  for (std::size_t c : data.classes) {
    if (c >= data.target.labels.size()) {
      throw std::invalid_argument("inslv: clase " + std::to_string(c) + " fuera de rango");
    }
  }
}

// Desvía std::cout a ninguna parte mientras existe: la implementación original escribe su
// progreso por la salida estándar.
class SilenceStdout {
 public:
  explicit SilenceStdout(bool silence) : previous_(silence ? std::cout.rdbuf(&null_) : nullptr) {}
  ~SilenceStdout() {
    if (previous_ != nullptr) {
      std::cout.rdbuf(previous_);
    }
  }
  SilenceStdout(const SilenceStdout&) = delete;
  SilenceStdout& operator=(const SilenceStdout&) = delete;

 private:
  class NullBuffer : public std::streambuf {
   protected:
    int overflow(int c) override { return traits_type::not_eof(c); }
  };

  NullBuffer null_;
  std::streambuf* previous_;
};

}  // namespace

std::vector<Rule> learn(const Dataset& data, const Options& options) {
  check(data);

  // example_set recibe las columnas de todas las variables, con la clase al final. Los
  // desconocidos se marcan con missing_value y cada clase, con el centro de su etiqueta.
  std::vector<std::vector<double>> columns = data.values;
  for (std::vector<double>& column : columns) {
    for (double& value : column) {
      if (std::isnan(value)) {
        value = missing_value;
      }
    }
  }
  std::vector<double> classes;
  classes.reserve(data.classes.size());
  for (std::size_t c : data.classes) {
    const FuzzySet& label = data.target.labels[c];
    classes.push_back((label.b + label.c) / 2.0);
  }
  columns.push_back(std::move(classes));

  VectorVar variables(data);
  example_set examples(&columns);

  SilenceStdout silence(!options.verbose);
  return Aprender(variables, examples, options);
}

}  // namespace inslv
