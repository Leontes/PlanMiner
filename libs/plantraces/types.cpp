#include "plantraces/types.hpp"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <utility>

namespace plantraces {

namespace {

// Los pares (posición, tipo) que se recogen de una acción o un predicado, en orden.
using Uses = std::vector<std::pair<std::size_t, std::string>>;

const std::string& type_of(const Trace& trace, const std::string& object) {
  return trace.objects.at(object).front();
}

// Sin argumentos no hay cabecera, como en el original.
void collect(std::map<std::string, Uses>& all, const std::string& name, const Trace& trace,
             const std::vector<std::string>& arguments) {
  for (std::size_t p = 0; p < arguments.size(); ++p) {
    all[name].emplace_back(p, type_of(trace, arguments[p]));
  }
}

// Los átomos conocidos (ciertos y falsos, en orden) y después los fluentes con valor.
void collect_state(std::map<std::string, Uses>& predicates, const Trace& trace,
                   const Observation& state) {
  if (state.missing) {
    return;
  }
  std::vector<GroundAtom> known(state.true_atoms.begin(), state.true_atoms.end());
  known.insert(known.end(), state.false_atoms.begin(), state.false_atoms.end());
  std::sort(known.begin(), known.end());
  for (const GroundAtom& atom : known) {
    collect(predicates, atom.name, trace, atom.arguments);
  }
  for (const auto& [term, value] : state.values) {
    collect(predicates, term.name, trace, term.arguments);
  }
}

void unique(std::map<std::string, Uses>& all) {
  for (auto& [name, uses] : all) {
    Uses first;
    for (const auto& use : uses) {
      if (std::find(first.begin(), first.end(), use) == first.end()) {
        first.push_back(use);
      }
    }
    uses = std::move(first);
  }
}

class Hierarchy {
 public:
  std::size_t index(const std::string& name) const {
    return static_cast<std::size_t>(
        std::find_if(types_.begin(), types_.end(), [&](const Node& t) { return t.name == name; }) -
        types_.begin());
  }

  void add(const std::string& name) {
    if (index(name) == types_.size()) {
      types_.push_back({name, std::nullopt});
    }
  }

  // Los tipos de cada parámetro o argumento y los conflictos entre ellos.
  void scan(const std::map<std::string, Uses>& all) {
    for (const auto& [name, uses] : all) {
      for (std::size_t i = 0; i < uses.size(); ++i) {
        add(uses[i].second);
        for (std::size_t j = i; j < uses.size(); ++j) {
          if (uses[i].first == uses[j].first && uses[i].second != uses[j].second) {
            conflicts_.emplace_back(uses[i].second, uses[j].second);
          }
        }
      }
    }
  }

  void resolve() {
    for (std::size_t i = 0; i < conflicts_.size(); ++i) {
      const std::size_t first = index(conflicts_[i].first);
      const std::size_t second = index(conflicts_[i].second);
      const std::optional<std::size_t> first_parent = types_[first].parent;
      const std::optional<std::size_t> second_parent = types_[second].parent;
      if (!first_parent && !second_parent) {
        types_.push_back({"PARENTTYPE" + std::to_string(i), std::nullopt});
        types_[first].parent = types_.size() - 1;
        types_[second].parent = types_.size() - 1;
      } else if (first_parent && second_parent && *first_parent != *second_parent) {
        conflicts_.emplace_back(types_[*first_parent].name, types_[*second_parent].name);
      } else {
        if (types_[first].parent) {
          types_[second].parent = types_[first].parent;
        }
        if (types_[second].parent) {
          types_[first].parent = types_[second].parent;
        }
      }
    }
  }

  // (solveConflict) El primer antecesor de a (contando a) que también lo es de b.
  std::string common(const std::string& a, const std::string& b) const {
    for (std::optional<std::size_t> x = index(a); x; x = types_[*x].parent) {
      for (std::optional<std::size_t> y = index(b); y; y = types_[*y].parent) {
        if (types_[*x].name == types_[*y].name) {
          return types_[*x].name;
        }
      }
    }
    return "";
  }

  // El tipo de cada posición, en el orden en que aparecen.
  std::map<std::string, std::vector<std::string>> headers(
      const std::map<std::string, Uses>& all) const {
    std::map<std::string, std::vector<std::string>> result;
    for (const auto& [name, uses] : all) {
      std::vector<std::size_t> positions;
      std::vector<std::string>& header = result[name];
      for (std::size_t j = 0; j < uses.size(); ++j) {
        std::optional<std::string> resolved;
        for (std::size_t k = j; k < uses.size(); ++k) {
          if (uses[j].first == uses[k].first && uses[j].second != uses[k].second) {
            resolved = common(uses[j].second, uses[k].second);
          }
        }
        if (std::find(positions.begin(), positions.end(), uses[j].first) == positions.end()) {
          positions.push_back(uses[j].first);
          header.push_back(resolved ? *resolved : uses[j].second);
        }
      }
    }
    return result;
  }

  std::vector<Type> types() const {
    std::vector<Type> result;
    for (const Node& node : types_) {
      result.push_back({node.name, node.parent ? types_[*node.parent].name : ""});
    }
    return result;
  }

 private:
  struct Node {
    std::string name;
    std::optional<std::size_t> parent;
  };
  std::vector<Node> types_;
  std::vector<std::pair<std::string, std::string>> conflicts_;
};

}  // namespace

TypeHierarchy type_hierarchy(const std::vector<Trace>& traces) {
  std::map<std::string, Uses> actions;
  std::map<std::string, Uses> predicates;
  for (const Trace& trace : traces) {
    for (const GroundAction& action : trace.actions) {
      collect(actions, action.name, trace, action.arguments);
      for (const std::size_t step : {action.step - 1, action.step}) {
        if (step < trace.states.size()) {
          collect_state(predicates, trace, trace.states[step]);
        }
      }
    }
  }
  unique(actions);
  unique(predicates);

  Hierarchy hierarchy;
  hierarchy.scan(actions);
  hierarchy.scan(predicates);
  hierarchy.resolve();
  return {hierarchy.types(), hierarchy.headers(actions), hierarchy.headers(predicates)};
}

}  // namespace plantraces
