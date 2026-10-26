#include "legacy_rules.hpp"

#include <cstddef>

#include "DataSLib.hpp"
#include "PtLib.hpp"
#include "RuleLib.hpp"

namespace oracle {

std::map<std::string, std::array<std::optional<LegacyExamples>, 2>> legacy_rules(
    const std::filesystem::path& trace) {
  // El original no libera las trazas que devuelve.
  std::vector<PlanTrace*>* plans = parse(trace.c_str());
  for (PlanTrace* plan : *plans) {
    plan->schematize();
  }
  std::map<std::string, StatesLists> grouped = groupTaskStates(plans);

  std::map<std::string, std::vector<std::vector<std::vector<double>>>> tables;
  std::map<std::string, std::vector<std::vector<std::pair<std::string, std::string>>>> labels;
  for (auto& [task, states] : grouped) {
    to_table(task, states, &tables[task], &labels[task]);
  }

  std::map<std::string, std::array<std::optional<LegacyExamples>, 2>> result;
  for (auto& [task, task_tables] : tables) {
    for (std::size_t kind = 0; kind < 2; ++kind) {
      std::vector<std::vector<double>>& table = task_tables[kind];
      std::vector<std::pair<std::string, std::string>>& names = labels[task][kind];
      if (names.size() <= 1) {
        continue;
      }
      LegacyExamples examples;
      std::map<std::string, std::vector<FuzzSet>> sets = discretize(&table, &names);
      inslv("oracle", task, "./", &table, names, sets, &examples.rules);
      for (std::size_t a = 0; a + 1 < names.size(); ++a) {
        examples.names.push_back(names[a].first);
        std::vector<LegacyLabel> attribute_labels;
        for (const FuzzSet& set : sets.at(names[a].first)) {
          attribute_labels.push_back({set.label, set.a, set.b, set.c, set.d});
        }
        examples.labels.push_back(attribute_labels);
        examples.values.push_back(table[a]);
      }
      examples.classes = table.back();
      result[task][kind] = std::move(examples);
    }
  }
  return result;
}

}  // namespace oracle
