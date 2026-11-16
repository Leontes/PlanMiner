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

double value_changes(const std::vector<Trace>& traces) {
  std::size_t observed = 0;
  std::size_t changed = 0;
  for (const Trace& trace : traces) {
    for (std::size_t i = 0; i < trace.actions.size(); ++i) {
      const std::vector<std::string>& arguments = trace.actions[i].arguments;
      const auto outside = [&arguments](const GroundAtom& term) {
        return !term.arguments.empty() &&
               std::none_of(term.arguments.begin(), term.arguments.end(),
                            [&arguments](const std::string& object) {
                              return std::find(arguments.begin(), arguments.end(), object) !=
                                     arguments.end();
                            });
      };
      for (const auto& [term, was] : trace.states[i].values) {
        const std::optional<double> is = trace.states[i + 1].value(term);
        if (outside(term) && is) {
          ++observed;
          changed += *is != was ? 1U : 0U;
        }
      }
    }
  }
  return observed == 0 ? 0.0 : static_cast<double>(changed) / static_cast<double>(observed);
}

double value_noise(double value_changes) {
  return value_changes < 1.0 ? 1.0 - std::sqrt(1.0 - value_changes) : 1.0;
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

// Lo que mantiene cada acción: de cada término (átomo o fluente) sobre sus parámetros, si lo que
// cambia entre los estados de antes y de después, en las aplicaciones en que se conoce en los dos,
// lo explica el ruido: hasta binomial_bound(n, q, alpha) de las n. terms da los términos de una
// traza y read, lo que se observa de un término en un estado.
template <typename Terms, typename Read>
std::map<ActionAtom, bool> kept_by_actions(const std::vector<Trace>& traces, Terms terms, Read read,
                                           double q, double alpha) {
  std::map<ActionAtom, std::pair<std::size_t, std::size_t>> changes;  // conocidas, cambian
  for (const Trace& trace : traces) {
    for (const GroundAtom& term : terms(trace)) {
      for (std::size_t i = 0; i < trace.actions.size(); ++i) {
        const auto parameters = parameters_of(term, trace.actions[i]);
        if (!parameters || parameters->empty()) {
          continue;
        }
        const auto was = read(trace.states[i], term);
        const auto is = read(trace.states[i + 1], term);
        if (was && is) {
          auto& [known, changed] = changes[{trace.actions[i].name, {term.name, *parameters}}];
          ++known;
          changed += *was != *is ? 1U : 0U;
        }
      }
    }
  }
  std::map<ActionAtom, bool> keeps;
  for (const auto& [action_term, counts] : changes) {
    const auto [known, changed] = counts;
    keeps[action_term] = changed <= binomial_bound(known, q, alpha);
  }
  return keeps;
}

// Los tramos de estados [first, last] de la traza en que el término no cambia: los cortan las
// acciones que pueden cambiarlo (las que no lo mantienen según keeps, las que lo tienen sobre un
// objeto que ocupa varios parámetros y las que no tienen datos). Una acción con un cambio
// constante (steps) no corta: une los dos tramos en una cadena, y offsets[s] es lo que han sumado
// esos cambios hasta el estado s. segment recibe cada cadena (first, last, offsets).
template <typename Segment>
void for_each_segment(const Trace& trace, const GroundAtom& term,
                      const std::map<ActionAtom, bool>& keeps,
                      const std::map<ActionAtom, double>& steps, Segment segment) {
  const std::size_t states = trace.states.size();
  std::vector<double> offsets(states, 0.0);
  double offset = 0.0;
  std::size_t first = 0;
  for (std::size_t s = 0; s < states; ++s) {
    offsets[s] = offset;
    if (s + 1 == states) {
      segment(first, s, offsets);
      break;
    }
    const GroundAction& action = trace.actions[s];
    const auto parameters = parameters_of(term, action);
    if (!parameters) {
      continue;
    }
    const ActionAtom key{action.name, {term.name, *parameters}};
    if (!parameters->empty()) {
      if (const auto kept = keeps.find(key); kept != keeps.end() && kept->second) {
        continue;
      }
      if (const auto step = steps.find(key); step != steps.end()) {
        offset += step->second;
        continue;
      }
    }
    segment(first, s, offsets);
    first = s + 1;
    offset = 0.0;
  }
}

}  // namespace

ColumnNoise smooth_atoms(std::vector<Trace>& traces, double p, double effect_alpha) {
  // 1. Qué mantiene cada acción: entre dos observaciones del mismo valor, el ruido cambia
  // 2p(1 - p) de ellas.
  const std::map<ActionAtom, bool> keeps = kept_by_actions(
      traces, atoms_with_arguments,
      [](const Observation& state, const GroundAtom& atom) { return state.truth(atom); },
      2.0 * p * (1.0 - p), effect_alpha);

  // 2 y 3. Los tramos de cada átomo y su suavizado; y, de las observaciones de antes y de después
  // de cada acción, la suma de las probabilidades de que estén mal, y cuántas son.
  std::map<ActionAtom, std::array<std::pair<double, std::size_t>, 2>> sums;
  for (Trace& trace : traces) {
    const std::size_t states = trace.states.size();
    for (const GroundAtom& atom : atoms_with_arguments(trace)) {
      std::vector<std::optional<double>> wrong(states);
      for_each_segment(trace, atom, keeps, {},
                       [&](std::size_t first, std::size_t last, const std::vector<double>&) {
                         smooth_segment(trace, atom, first, last, p, wrong);
                       });
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

namespace {

// Los fluentes con argumentos que aparecen en algún estado de la traza.
std::set<GroundAtom> fluents_with_arguments(const Trace& trace) {
  std::set<GroundAtom> terms;
  for (const Observation& state : trace.states) {
    for (const auto& [term, value] : state.values) {
      if (!term.arguments.empty()) {
        terms.insert(term);
      }
    }
    for (const GroundAtom& term : state.unknown_values) {
      if (!term.arguments.empty()) {
        terms.insert(term);
      }
    }
  }
  return terms;
}

// El suavizado del fluente en la cadena de estados [first, last] de la traza, en que su valor menos
// offsets[s] es el mismo (smooth_values, paso 3).
void smooth_value_segment(Trace& trace, const GroundAtom& term, std::size_t first, std::size_t last,
                          const std::vector<double>& offsets) {
  std::map<double, std::size_t> counts;
  for (std::size_t s = first; s <= last; ++s) {
    if (const std::optional<double> value = trace.states[s].value(term)) {
      ++counts[*value - offsets[s]];
    }
  }
  // La moda, si sale al menos dos veces y más que cualquier otro valor.
  std::optional<double> mode;
  std::size_t best = 0;
  std::size_t second = 0;
  for (const auto& [value, count] : counts) {
    if (count > best) {
      second = best;
      best = count;
      mode = value;
    } else if (count > second) {
      second = count;
    }
  }
  if (best < 2 || best == second) {
    mode.reset();
  }
  for (std::size_t s = first; s <= last; ++s) {
    Observation& state = trace.states[s];
    const auto found = state.values.find(term);
    if (found == state.values.end()) {
      continue;
    }
    if (mode) {
      found->second = *mode + offsets[s];
    } else {
      state.values.erase(found);
      state.unknown_values.insert(term);
    }
  }
}

// Los fluentes sin argumentos que ninguna acción cambia en más de la mitad de sus aplicaciones
// (en que se conocen antes y después): un efecto cambia el fluente casi siempre, y el ruido, en
// menos de la mitad. No se usa el ruido estimado: sale de los fluentes con argumentos, y en los
// enteros pequeños la alteración se pierde al redondear (en counters, max_int, de unos 22, cambia
// más que los contadores).
std::set<std::string> constants_without_arguments(const std::vector<Trace>& traces) {
  // De cada fluente y acción, las aplicaciones en que se conoce antes y después y en cuántas
  // cambia.
  std::map<std::string, std::map<std::string, std::pair<std::size_t, std::size_t>>> changes;
  for (const Trace& trace : traces) {
    for (const Observation& state : trace.states) {
      for (const auto& [term, value] : state.values) {
        if (term.arguments.empty()) {
          changes.try_emplace(term.name);
        }
      }
    }
    for (std::size_t i = 0; i < trace.actions.size(); ++i) {
      for (const auto& [term, was] : trace.states[i].values) {
        if (!term.arguments.empty()) {
          continue;
        }
        if (const std::optional<double> is = trace.states[i + 1].value(term)) {
          auto& [known, changed] = changes[term.name][trace.actions[i].name];
          ++known;
          changed += *is != was ? 1U : 0U;
        }
      }
    }
  }
  std::set<std::string> constants;
  for (const auto& [fluent, actions] : changes) {
    if (std::all_of(actions.begin(), actions.end(), [](const auto& entry) {
          const auto [known, changed] = entry.second;
          return 2 * changed <= known;
        })) {
      constants.insert(fluent);
    }
  }
  return constants;
}

// Los cambios constantes de las acciones que cambian cada fluente según keeps: el que sale en más
// de la mitad de sus aplicaciones (en que se conoce antes y después), al menos dos veces. Con
// ruido, los cambios alterados salen casi siempre distintos.
std::map<ActionAtom, double> constant_steps(const std::vector<Trace>& traces,
                                            const std::map<ActionAtom, bool>& keeps) {
  std::map<ActionAtom, std::map<double, std::size_t>> changes;
  for (const Trace& trace : traces) {
    for (const GroundAtom& term : fluents_with_arguments(trace)) {
      for (std::size_t i = 0; i < trace.actions.size(); ++i) {
        const auto parameters = parameters_of(term, trace.actions[i]);
        if (!parameters || parameters->empty()) {
          continue;
        }
        const ActionAtom key{trace.actions[i].name, {term.name, *parameters}};
        if (const auto kept = keeps.find(key); kept == keeps.end() || kept->second) {
          continue;
        }
        const std::optional<double> was = trace.states[i].value(term);
        const std::optional<double> is = trace.states[i + 1].value(term);
        if (was && is) {
          ++changes[key][*is - *was];
        }
      }
    }
  }
  std::map<ActionAtom, double> steps;
  for (const auto& [key, counts] : changes) {
    std::size_t total = 0;
    std::size_t best = 0;
    double step = 0.0;
    for (const auto& [change, count] : counts) {
      total += count;
      if (count > best) {
        best = count;
        step = change;
      }
    }
    if (best >= 2 && 2 * best > total) {
      steps[key] = step;
    }
  }
  return steps;
}

}  // namespace

std::set<std::string> smooth_values(std::vector<Trace>& traces, double p, bool chains,
                                    double effect_alpha) {
  // 1. Qué mantiene cada acción: dos observaciones del mismo valor difieren si alguna está
  // alterada, 1 - (1 - p)^2.
  const std::map<ActionAtom, bool> keeps = kept_by_actions(
      traces, fluents_with_arguments,
      [](const Observation& state, const GroundAtom& term) { return state.value(term); },
      1.0 - (1.0 - p) * (1.0 - p), effect_alpha);
  const std::map<ActionAtom, double> steps =
      chains ? constant_steps(traces, keeps) : std::map<ActionAtom, double>{};
  const std::set<std::string> constants = constants_without_arguments(traces);

  // 2 y 3. Los tramos (o las cadenas) de cada fluente y su suavizado; de las constantes, la traza
  // entera.
  std::set<std::string> smoothed = constants;
  for (Trace& trace : traces) {
    for (const GroundAtom& term : fluents_with_arguments(trace)) {
      smoothed.insert(term.name);
      for_each_segment(
          trace, term, keeps, steps,
          [&](std::size_t first, std::size_t last, const std::vector<double>& offsets) {
            smooth_value_segment(trace, term, first, last, offsets);
          });
    }
    if (!trace.states.empty()) {
      const std::vector<double> none(trace.states.size(), 0.0);
      for (const std::string& constant : constants) {
        smooth_value_segment(trace, GroundAtom{constant, {}}, 0, trace.states.size() - 1, none);
      }
    }
  }
  return smoothed;
}

}  // namespace plantraces
