#include "plantraces/noise.hpp"

#include <algorithm>
#include <cmath>
#include <optional>
#include <set>
#include <string>

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

}  // namespace plantraces
