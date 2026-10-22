#include "pts.hpp"

#include <map>
#include <sstream>
#include <stdexcept>

namespace oracle {

namespace {

std::string typed(const std::string& object, const plantraces::Trace& trace) {
  const std::vector<std::string>& types = trace.objects.at(object);
  if (types.size() != 1) {
    throw std::invalid_argument("el formato .pts no admite objetos con varios tipos: " + object);
  }
  return object + " - " + types.front();
}

std::string typed(const plantraces::GroundAtom& atom, const plantraces::Trace& trace) {
  std::string text = "(" + atom.name;
  for (const std::string& argument : atom.arguments) {
    text += " " + typed(argument, trace);
  }
  return text + ")";
}

std::string number(double value) {
  std::ostringstream out;
  out.precision(17);
  out << value;
  return out.str();
}

}  // namespace

std::string to_pts(const std::vector<plantraces::Trace>& traces) {
  std::ostringstream out;
  for (const plantraces::Trace& trace : traces) {
    out << "New plan!!!\n\n##Tasks##\n";
    for (const plantraces::GroundAction& action : trace.actions) {
      out << "[" << action.step - 1 << ", " << action.step
          << "]: " << typed(plantraces::GroundAtom{action.name, action.arguments}, trace) << "\n";
    }

    out << "\n\n##States##\n";
    for (const plantraces::Observation& state : trace.states) {
      if (state.missing) {
        continue;
      }
      std::map<plantraces::GroundAtom, bool> known;
      for (const plantraces::GroundAtom& atom : state.true_atoms) {
        known.emplace(atom, true);
      }
      for (const plantraces::GroundAtom& atom : state.false_atoms) {
        known.emplace(atom, false);
      }

      out << "[" << state.step << "]: (";
      bool first = true;
      const auto separate = [&out, &first] {
        if (!first) {
          out << ' ';
        }
        first = false;
      };
      for (const auto& [atom, truth] : known) {
        separate();
        out << (truth ? typed(atom, trace) : "(not " + typed(atom, trace) + ")");
      }
      for (const auto& [term, value] : state.values) {
        separate();
        out << "(= " << typed(term, trace) << " " << number(value) << ")";
      }
      out << ")\n\n";
    }
    out << "\n\n";
  }
  return out.str();
}

}  // namespace oracle
