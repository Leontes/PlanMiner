// Las tablas de las acciones de las trazas de prueba (tests/data/traces), para los tests de lo que
// se extrae de ellas.
#pragma once

#include <algorithm>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "datasets/table.hpp"
#include "plantraces/actions.hpp"
#include "plantraces/trace.hpp"

namespace datasets::test {

/// Las trazas de una variante de una colección ("zenotravel", "complete"), en orden.
inline std::vector<plantraces::Trace> read_traces(const std::string& collection,
                                                  const std::string& variant) {
  namespace fs = std::filesystem;
  std::vector<fs::path> files;
  for (const auto& entry : fs::directory_iterator(fs::path{PLANMINER_TRACES_DIR} / collection)) {
    if (entry.path().filename().string().ends_with("." + variant + ".json")) {
      files.push_back(entry.path());
    }
  }
  std::sort(files.begin(), files.end());
  std::vector<plantraces::Trace> traces;
  for (const fs::path& file : files) {
    traces.push_back(plantraces::read_trace(file));
  }
  return traces;
}

/// Las tablas de las acciones de una variante de una colección.
class Tables {
 public:
  Tables(const std::string& collection, const std::string& variant)
      : traces_(read_traces(collection, variant)) {
    const plantraces::Vocabulary vocabulary = plantraces::vocabulary(traces_);
    for (const auto& [name, transitions] : plantraces::group_by_action(traces_)) {
      tables_.emplace(name, build_table(vocabulary, transitions));
    }
  }
  const Table& at(const std::string& action) const { return tables_.at(action); }
  const std::map<std::string, Table>& all() const { return tables_; }

 private:
  std::vector<plantraces::Trace> traces_;
  std::map<std::string, Table> tables_;
};

}  // namespace datasets::test
