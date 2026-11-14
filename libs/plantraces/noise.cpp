#include "plantraces/noise.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "plantraces/actions.hpp"

namespace plantraces {

double frame_changes(const std::vector<Trace>& traces) {
  std::size_t observed = 0;
  std::size_t changed = 0;
  for (const auto& [name, transitions] : group_by_action(traces)) {
    for (const Transition& transition : transitions) {
      const std::vector<std::string>& arguments = transition.action->arguments;
      const auto outside = [&arguments](const GroundAtom& atom) {
        return !atom.arguments.empty() &&
               std::none_of(atom.arguments.begin(), atom.arguments.end(),
                            [&arguments](const std::string& object) {
                              return std::find(arguments.begin(), arguments.end(), object) !=
                                     arguments.end();
                            });
      };
      const Observation& before = *transition.before;
      const Observation& after = *transition.after;
      std::set<GroundAtom> atoms;
      std::set<GroundAtom> terms;
      for (const Observation* state : {&before, &after}) {
        atoms.insert(state->true_atoms.begin(), state->true_atoms.end());
        atoms.insert(state->false_atoms.begin(), state->false_atoms.end());
        for (const auto& [term, value] : state->values) {
          terms.insert(term);
        }
      }
      for (const GroundAtom& atom : atoms) {
        const std::optional<bool> was = before.truth(atom);
        const std::optional<bool> is = after.truth(atom);
        if (outside(atom) && was && is) {
          ++observed;
          changed += *was != *is ? 1U : 0U;
        }
      }
      for (const GroundAtom& term : terms) {
        const std::optional<double> was = before.value(term);
        const std::optional<double> is = after.value(term);
        if (outside(term) && was && is) {
          ++observed;
          changed += *was != *is ? 1U : 0U;
        }
      }
    }
  }
  return observed == 0 ? 0.0 : static_cast<double>(changed) / static_cast<double>(observed);
}

double observation_noise(double frame_changes) {
  return frame_changes < 0.5 ? (1.0 - std::sqrt(1.0 - 2.0 * frame_changes)) / 2.0 : 0.5;
}

std::size_t binomial_bound(std::size_t n, double p, double alpha) {
  if (p <= 0.0) {
    return 0;
  }
  if (p >= 1.0) {
    return n;
  }
  // La función de distribución, término a término en logaritmos: (1 - p)^n se sale del double.
  const double trials = static_cast<double>(n);
  double cumulative = 0.0;
  for (std::size_t m = 0; m < n; ++m) {
    const double k = static_cast<double>(m);
    cumulative +=
        std::exp(std::lgamma(trials + 1.0) - std::lgamma(k + 1.0) - std::lgamma(trials - k + 1.0) +
                 k * std::log(p) + (trials - k) * std::log1p(-p));
    if (cumulative >= 1.0 - alpha) {
      return m;
    }
  }
  return n;
}

namespace {

// Los parámetros de la acción con los argumentos del átomo, si la acción puede cambiarlo: nullopt
// si alguno no es de la acción; vacío si alguno ocupa varios parámetros (puede cambiarlo, pero el
// átomo no tiene una forma única sobre los parámetros).
std::optional<std::vector<std::size_t>> parameters_of(const GroundAtom& atom,
                                                      const GroundAction& action) {
  const std::vector<std::string>& arguments = action.arguments;
  std::vector<std::size_t> parameters;
  for (const std::string& object : atom.arguments) {
    const auto first = std::find(arguments.begin(), arguments.end(), object);
    if (first == arguments.end()) {
      return std::nullopt;
    }
    if (std::find(first + 1, arguments.end(), object) != arguments.end()) {
      return std::vector<std::size_t>{};
    }
    parameters.push_back(static_cast<std::size_t>(first - arguments.begin()));
  }
  return parameters;
}

// Los átomos con argumentos que aparecen en algún estado de la traza (en mundo cerrado, los falsos
// en todos no aparecen: no hay nada que suavizar).
std::set<GroundAtom> atoms_with_arguments(const Trace& trace) {
  std::set<GroundAtom> atoms;
  for (const Observation& state : trace.states) {
    for (const auto* listed : {&state.true_atoms, &state.false_atoms, &state.unknown_atoms}) {
      for (const GroundAtom& atom : *listed) {
        if (!atom.arguments.empty()) {
          atoms.insert(atom);
        }
      }
    }
  }
  return atoms;
}

// Cambia lo que se observa del átomo en el estado: cierto, falso o, con nullopt, desconocido. En
// mundo cerrado, los falsos no se listan.
void observe(Observation& state, const GroundAtom& atom, std::optional<bool> value, World world) {
  state.true_atoms.erase(atom);
  state.false_atoms.erase(atom);
  state.unknown_atoms.erase(atom);
  if (!value) {
    state.unknown_atoms.insert(atom);
  } else if (*value) {
    state.true_atoms.insert(atom);
  } else if (world == World::open) {
    state.false_atoms.insert(atom);
  }
}

// El suavizado del átomo en el tramo de estados [first, last] de la traza, en que su valor es el
// mismo (smooth_atoms, paso 3). En wrong, la probabilidad de que esté mal cada observación que
// queda conocida.
void smooth_segment(Trace& trace, const GroundAtom& atom, std::size_t first, std::size_t last,
                    double p, std::vector<std::optional<double>>& wrong) {
  std::size_t known = 0;
  std::size_t holds = 0;
  for (std::size_t s = first; s <= last; ++s) {
    if (const std::optional<bool> value = trace.states[s].truth(atom)) {
      ++known;
      holds += *value ? 1U : 0U;
    }
  }
  const std::size_t against = known - holds;
  const std::size_t margin = holds > against ? holds - against : against - holds;
  const bool majority = holds > against;
  for (std::size_t s = first; s <= last; ++s) {
    const std::optional<bool> value = trace.states[s].truth(atom);
    if (!value) {
      continue;
    }
    if (known == 1) {
      wrong[s] = p;
    } else if (margin <= 1) {
      observe(trace.states[s], atom, std::nullopt, trace.world);
    } else {
      if (*value != majority) {
        observe(trace.states[s], atom, majority, trace.world);
      }
      wrong[s] = 1.0 / (1.0 + std::pow((1.0 - p) / p, static_cast<double>(margin)));
    }
  }
}

}  // namespace

ColumnNoise smooth_atoms(std::vector<Trace>& traces, double p, double effect_alpha) {
  // 1. Qué cambia cada acción: de cada átomo sobre sus parámetros, las aplicaciones en que se
  // conoce antes y después, y en cuántas cambia.
  std::map<ActionAtom, std::pair<std::size_t, std::size_t>> changes;
  for (const Trace& trace : traces) {
    for (const GroundAtom& atom : atoms_with_arguments(trace)) {
      for (std::size_t i = 0; i < trace.actions.size(); ++i) {
        const auto parameters = parameters_of(atom, trace.actions[i]);
        if (!parameters || parameters->empty()) {
          continue;
        }
        const std::optional<bool> was = trace.states[i].truth(atom);
        const std::optional<bool> is = trace.states[i + 1].truth(atom);
        if (was && is) {
          auto& [known, changed] = changes[{trace.actions[i].name, {atom.name, *parameters}}];
          ++known;
          changed += *was != *is ? 1U : 0U;
        }
      }
    }
  }
  // Las que lo mantienen: entre dos observaciones del mismo valor, el ruido cambia
  // 2p(1 - p) de ellas.
  const double q = 2.0 * p * (1.0 - p);
  std::map<ActionAtom, bool> keeps;
  for (const auto& [action_atom, counts] : changes) {
    const auto [known, changed] = counts;
    keeps[action_atom] = changed <= binomial_bound(known, q, effect_alpha);
  }
  const auto cuts = [&keeps](const GroundAtom& atom, const GroundAction& action) {
    const auto parameters = parameters_of(atom, action);
    if (!parameters) {
      return false;
    }
    if (parameters->empty()) {
      return true;
    }
    const auto found = keeps.find({action.name, {atom.name, *parameters}});
    return found == keeps.end() || !found->second;
  };

  // 2 y 3. Los tramos de cada átomo y su suavizado; y, de las observaciones de antes y de después
  // de cada acción, la suma de las probabilidades de que estén mal, y cuántas son.
  std::map<ActionAtom, std::array<std::pair<double, std::size_t>, 2>> sums;
  for (Trace& trace : traces) {
    const std::size_t states = trace.states.size();
    for (const GroundAtom& atom : atoms_with_arguments(trace)) {
      std::vector<std::optional<double>> wrong(states);
      std::size_t first = 0;
      for (std::size_t s = 0; s < states; ++s) {
        if (s + 1 == states || cuts(atom, trace.actions[s])) {
          smooth_segment(trace, atom, first, s, p, wrong);
          first = s + 1;
        }
      }
      for (std::size_t i = 0; i < trace.actions.size(); ++i) {
        const auto parameters = parameters_of(atom, trace.actions[i]);
        if (!parameters || parameters->empty()) {
          continue;
        }
        auto& column = sums[{trace.actions[i].name, {atom.name, *parameters}}];
        for (std::size_t side = 0; side < 2; ++side) {
          if (const std::optional<double> probability = wrong[i + side]) {
            column[side].first += *probability;
            ++column[side].second;
          }
        }
      }
    }
  }

  ColumnNoise noise;
  for (const auto& [action_atom, column] : sums) {
    for (std::size_t side = 0; side < 2; ++side) {
      const auto [sum, count] = column[side];
      noise[action_atom][side] = count > 0 ? sum / static_cast<double>(count) : p;
    }
  }
  return noise;
}

}  // namespace plantraces
