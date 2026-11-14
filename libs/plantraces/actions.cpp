#include "plantraces/actions.hpp"

#include <algorithm>
#include <optional>
#include <set>
#include <string>
#include <string_view>

namespace plantraces {

namespace {

// Añade a la firma del símbolo los tipos con que aparecen los argumentos del átomo.
void observe(std::map<std::string, Signature>& symbols, const GroundAtom& atom, const Trace& trace,
             std::string_view kind) {
  const std::size_t arity = atom.arguments.size();
  const auto [entry, inserted] =
      symbols.try_emplace(atom.name, Signature{arity, std::vector<std::set<std::string>>(arity)});
  Signature& signature = entry->second;
  if (signature.arity != arity) {
    throw TraceError(std::string{kind} + " '" + atom.name + "' aparece con " +
                     std::to_string(signature.arity) + " y con " + std::to_string(arity) +
                     " argumentos");
  }
  for (std::size_t k = 0; k < arity; ++k) {
    const std::vector<std::string>& types = trace.objects.at(atom.arguments[k]);
    signature.types[k].insert(types.begin(), types.end());
  }
}

bool fits(const std::vector<std::string>& object_types, const std::set<std::string>& allowed) {
  return std::any_of(object_types.begin(), object_types.end(),
                     [&allowed](const std::string& type) { return allowed.contains(type); });
}

}  // namespace

Vocabulary vocabulary(const std::vector<Trace>& traces) {
  Vocabulary result;
  for (const Trace& trace : traces) {
    for (const Observation& state : trace.states) {
      for (const auto* atoms : {&state.true_atoms, &state.false_atoms, &state.unknown_atoms}) {
        for (const GroundAtom& atom : *atoms) {
          observe(result.predicates, atom, trace, "el predicado");
        }
      }
      for (const auto& [term, value] : state.values) {
        observe(result.fluents, term, trace, "el fluente");
      }
      for (const GroundAtom& term : state.unknown_values) {
        observe(result.fluents, term, trace, "el fluente");
      }
    }
  }
  return result;
}

std::map<std::string, std::vector<Transition>> group_by_action(const std::vector<Trace>& traces) {
  std::map<std::string, std::vector<Transition>> result;
  for (const Trace& trace : traces) {
    for (std::size_t i = 0; i < trace.actions.size(); ++i) {
      const GroundAction& action = trace.actions[i];
      result[action.name].push_back({&trace, &action, &trace.states[i], &trace.states[i + 1]});
    }
  }
  return result;
}

std::string to_string(const LiftedAtom& atom) {
  std::string text = "(" + atom.name;
  for (const std::size_t parameter : atom.parameters) {
    text += " ?" + std::to_string(parameter);
  }
  return text + ")";
}

std::vector<Expressible> expressible(const std::map<std::string, Signature>& symbols,
                                     const Transition& transition) {
  const GroundAction& action = *transition.action;
  const Trace& trace = *transition.trace;

  // Los objetos de la acción, sin repetir, con el primer parámetro que los tiene.
  std::vector<std::pair<std::string, std::size_t>> objects;
  for (std::size_t parameter = 0; parameter < action.arguments.size(); ++parameter) {
    const std::string& object = action.arguments[parameter];
    if (std::none_of(objects.begin(), objects.end(),
                     [&object](const auto& known) { return known.first == object; })) {
      objects.emplace_back(object, parameter);
    }
  }

  std::vector<Expressible> result;
  for (const auto& [name, signature] : symbols) {
    if (signature.arity > 0 && objects.empty()) {
      continue;
    }
    // Todas las tuplas de objetos de la acción, como un cuentakilómetros sobre objects.
    std::vector<std::size_t> choice(signature.arity, 0);
    while (true) {
      Expressible candidate{{name, {}}, {name, {}}};
      bool typed = true;
      for (std::size_t k = 0; k < signature.arity && typed; ++k) {
        const auto& [object, parameter] = objects[choice[k]];
        typed = fits(trace.objects.at(object), signature.types[k]);
        candidate.ground.arguments.push_back(object);
        candidate.lifted.parameters.push_back(parameter);
      }
      if (typed) {
        result.push_back(std::move(candidate));
      }

      std::size_t k = signature.arity;
      while (k > 0 && ++choice[k - 1] == objects.size()) {
        choice[k - 1] = 0;
        --k;
      }
      if (k == 0) {
        break;
      }
    }
  }

  std::sort(result.begin(), result.end(),
            [](const Expressible& a, const Expressible& b) { return a.ground < b.ground; });
  return result;
}

}  // namespace plantraces
