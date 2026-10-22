#include "datasets/table.hpp"

#include <limits>
#include <map>
#include <optional>
#include <utility>

namespace datasets {

namespace {

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();

// Construye la tabla columna a columna: una columna se crea la primera vez que se conoce su valor.
class Builder {
 public:
  explicit Builder(std::size_t transitions) { table_.transitions = transitions; }

  void set(const Attribute& attribute, std::size_t row, double value) {
    auto [entry, inserted] = index_.try_emplace(attribute, table_.attributes.size());
    if (inserted) {
      table_.attributes.push_back(attribute);
      table_.columns.emplace_back(table_.rows(), unknown);
    }
    table_.columns[entry->second][row] = value;
  }

  Table take() { return std::move(table_); }

 private:
  Table table_;
  std::map<Attribute, std::size_t> index_;
};

}  // namespace

std::string to_string(const Attribute& attribute) {
  switch (attribute.kind) {
    case AttributeKind::atom:
      return plantraces::to_string(attribute.term);
    case AttributeKind::fluent:
      return "fluent " + plantraces::to_string(attribute.term);
    case AttributeKind::time:
      return "time";
  }
  return "";
}

Table build_table(const plantraces::Vocabulary& vocabulary,
                  const std::vector<plantraces::Transition>& transitions) {
  // Lo que se puede expresar con los parámetros de cada aplicación.
  std::vector<std::vector<plantraces::Expressible>> atoms;
  std::vector<std::vector<plantraces::Expressible>> fluents;
  for (const plantraces::Transition& transition : transitions) {
    atoms.push_back(plantraces::expressible(vocabulary.predicates, transition));
    fluents.push_back(plantraces::expressible(vocabulary.fluents, transition));
  }

  Builder builder(transitions.size());
  for (const bool after : {false, true}) {
    for (std::size_t i = 0; i < transitions.size(); ++i) {
      const plantraces::Observation& state = after ? *transitions[i].after : *transitions[i].before;
      const std::size_t row = after ? transitions.size() + i : i;

      for (const plantraces::Expressible& atom : atoms[i]) {
        if (const std::optional<bool> truth = state.truth(atom.ground)) {
          builder.set({AttributeKind::atom, atom.lifted}, row, *truth ? 1.0 : 0.0);
        }
      }
      for (const plantraces::Expressible& fluent : fluents[i]) {
        if (const std::optional<double> value = state.value(fluent.ground)) {
          builder.set({AttributeKind::fluent, fluent.lifted}, row, *value);
        }
      }
      builder.set({AttributeKind::time, {}}, row, state.time);
    }
  }
  return builder.take();
}

}  // namespace datasets
