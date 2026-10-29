#include "pddl/domain.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <utility>

#include "datasets/effects.hpp"
#include "datasets/relations.hpp"
#include "datasets/table.hpp"
#include "plantraces/actions.hpp"
#include "rules/legacy.hpp"

namespace pddl {

namespace {

std::vector<std::string> split(const std::string& text) {
  std::istringstream in(text);
  std::vector<std::string> tokens;
  for (std::string token; in >> token;) {
    tokens.push_back(token);
  }
  return tokens;
}

// Un token que puede faltar.
const std::string& token(const std::vector<std::string>& tokens, std::size_t i) {
  static const std::string none;
  return i < tokens.size() ? tokens[i] : none;
}

std::string lower(std::string text) {
  std::transform(text.begin(), text.end(), text.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return text;
}

const std::vector<std::string>& header_of(
    const std::map<std::string, std::vector<std::string>>& all, const std::string& name) {
  static const std::vector<std::string> none;
  const auto found = all.find(name);
  return found == all.end() ? none : found->second;
}

// Los tokens desde first, en minúsculas, sin "~" ni el tipo que le sigue.
std::string untyped(const std::vector<std::string>& tokens, std::size_t first) {
  std::string out;
  for (std::size_t i = first; i < tokens.size(); ++i) {
    if (tokens[i] != "~") {
      out += lower(tokens[i]) + " ";
    } else {
      ++i;
    }
  }
  return out;
}

void write_types(std::ostringstream& out, const plantraces::TypeHierarchy& hierarchy) {
  std::map<std::string, std::vector<std::string>> children;
  for (const plantraces::Type& type : hierarchy.types) {
    children[type.parent.empty() ? "object" : type.parent].push_back(type.name);
  }
  out << "\t(:types\n";
  for (const auto& [parent, names] : children) {
    out << "\t\t";
    for (const std::string& name : names) {
      out << lower(name) << " ";
    }
    out << "- " << lower(parent) << "\n";
  }
  out << "\t)\n";
}

// Los predicados (átomos) o las funciones (valores) de los ejemplos, en orden y sin repetir.
void write_symbols(std::ostringstream& out, const LearnedDomain& domain, bool functions,
                   std::vector<std::string>& written) {
  out << (functions ? "\t(:functions\n" : "\t(:predicates\n");
  for (const LearnedAction& action : domain.actions) {
    for (const rules::Examples* examples : {&action.logic, &action.numeric}) {
      for (const rules::Attribute& attribute : examples->attributes) {
        const bool wanted = functions ? attribute.kind == rules::AttributeKind::value &&
                                            attribute.term.kind == datasets::AttributeKind::fluent
                                      : attribute.kind == rules::AttributeKind::atom;
        if (!wanted) {
          continue;
        }
        const std::string& name = attribute.term.term.name;
        if (std::find(written.begin(), written.end(), name) != written.end()) {
          continue;
        }
        written.push_back(name);
        out << "\t\t(" << lower(name);
        const std::vector<std::string>& types = header_of(domain.hierarchy.predicates, name);
        for (std::size_t p = 0; p < types.size(); ++p) {
          out << " ?param_" << p << " - " << lower(types[p]);
        }
        out << ")\n";
      }
    }
  }
  out << "\t)\n";
}

void write_precondition(std::ostringstream& out, const std::vector<rules::Literal>& precondition,
                        bool equality) {
  out << "\t\t:precondition\n";
  out << (precondition.empty() ? "\t\t\t( )\n" : "\t\t\t(and\n");
  for (const rules::Literal& literal : precondition) {
    const std::vector<std::string> tokens = split(literal.attribute);
    if (token(tokens, 2) == "TIMESTAMP") {
      continue;
    }
    const std::string& op = tokens[0];
    const std::map<std::string, std::string> symbols = {{"opEQ", equality ? "= " : "== "},
                                                        {"opGET", ">= "},
                                                        {"opGT", "> "},
                                                        {"opLET", "<= "},
                                                        {"opLT", "< "}};
    if (const auto symbol = symbols.find(op); symbol != symbols.end()) {
      out << "\t\t\t\t(" << symbol->second;
      for (std::size_t i = 2; i < tokens.size(); ++i) {
        if (tokens[i] == "~" && token(tokens, i + 1) != "(") {
          ++i;
        } else {
          out << lower(tokens[i]) << " ";
        }
      }
      out << ")\n";
    } else if (literal.value != "False") {
      out << "\t\t\t\t" << untyped(tokens, 0) << "\n";
    }
  }
  if (!precondition.empty()) {
    out << "\t\t\t)\n";
  }
}

void write_effect(std::ostringstream& out, const std::vector<rules::Literal>& effect) {
  out << "\t\t:effect\n";
  out << (effect.empty() ? "\t\t\t( )\n" : "\t\t\t(and\n");
  for (const rules::Literal& literal : effect) {
    const std::vector<std::string> tokens = split(literal.attribute);
    const bool time = token(tokens, 2) == "TIMESTAMP";
    if (!time) {
      out << "\t\t\t\t";
    }
    if (tokens[0] == "DELTA") {
      if (!time) {
        out << (std::stod(literal.value) > 0.0 ? "( increase " : "( decrease ")
            << untyped(tokens, 2) << " )\n";
      }
    } else if (tokens[0] == "opEQ") {
      out << "( assign ( " << untyped(tokens, 2) << ")\n";
    } else {
      const bool negated = literal.value == "False";
      out << (negated ? "( not " : "") << untyped(tokens, 0) << (negated ? " )" : "") << "\n";
    }
  }
  if (!effect.empty()) {
    out << "\t\t\t)\n";
  }
}

// Un efecto de la regresión como lo escribe el original: un cambio con su signo o una asignación.
// Sin forma (solo el valor absoluto del cambio) no se sabe su sentido, y no se escribe.
std::optional<rules::Literal> literal(const datasets::NumericEffect& effect,
                                      const std::vector<std::string>& types) {
  const std::string fluent =
      rules::legacy_label({datasets::AttributeKind::fluent, effect.fluent}, types);
  const std::string formula = rules::legacy_formula(effect.expression, effect.variables, types);
  switch (effect.kind) {
    case datasets::EffectKind::increase:
      return rules::Literal{"DELTA - " + fluent + " " + formula + " ", "1"};
    case datasets::EffectKind::decrease:
      return rules::Literal{"DELTA - " + fluent + " " + formula + " ", "-1"};
    case datasets::EffectKind::assign:
      return rules::Literal{"opEQ " + fluent + " " + formula, formula};
    case datasets::EffectKind::magnitude:
      break;
  }
  return std::nullopt;
}

}  // namespace

Options Options::original() {
  Options options;
  options.effects = datasets::EffectOptions::original();
  options.relations = datasets::RelationOptions::original();
  options.examples = rules::ExampleOptions::original();
  options.fusion = rules::FusionOptions::original();
  options.numeric_fluents = false;
  options.equality = false;
  options.aliasing = false;
  return options;
}

LearnedDomain learn(const std::vector<plantraces::Trace>& traces, const Options& options) {
  LearnedDomain domain;
  domain.hierarchy = plantraces::type_hierarchy(traces);
  const plantraces::Vocabulary vocabulary = plantraces::vocabulary(traces);
  const std::set<std::string> accumulators = datasets::accumulators(traces);
  for (const auto& [name, transitions] : plantraces::group_by_action(traces)) {
    LearnedAction action;
    action.name = name;
    for (const std::string& object : transitions.front().action->arguments) {
      action.argument_types.push_back(transitions.front().trace->objects.at(object).front());
    }
    datasets::Table table = datasets::build_table(vocabulary, transitions);
    if (options.aliasing) {
      datasets::hide_aliased_atoms(table, transitions);
    }

    action.logic = rules::logic_examples(table);
    std::vector<rules::LegacyRule> rules =
        rules::legacy_rules(rules::learn(action.logic), action.logic, action.argument_types);

    const std::vector<datasets::FluentChange> changes =
        datasets::numeric_effects(table, options.effects);
    action.numeric = rules::numeric_examples(
        table, changes,
        datasets::numeric_relations(table, changes, options.relations, accumulators),
        [] { return std::rand(); }, options.examples);
    const std::vector<rules::LegacyRule> numeric =
        rules::legacy_rules(rules::learn(action.numeric), action.numeric, action.argument_types);
    rules.insert(rules.end(), numeric.begin(), numeric.end());

    action.fused = rules::fuse(
        rules, header_of(domain.hierarchy.actions, name),
        rules::evidence({&action.logic, &action.numeric}, action.argument_types), options.fusion);
    if (!options.examples.changes) {
      std::erase_if(action.fused.effect, [](const rules::Literal& literal) {
        const std::string op = token(split(literal.attribute), 0);
        return op == "DELTA" || op == "opEQ";
      });
      for (const datasets::FluentChange& change : changes) {
        if (change.effect) {
          action.effects.push_back(*change.effect);
        }
      }
    }
    domain.actions.push_back(std::move(action));
  }
  return domain;
}

std::string write(const std::string& name, const LearnedDomain& domain, const Options& options) {
  // Las secciones salen si hay atributos lógicos (átomos o relaciones) o numéricos (fluentes o el
  // instante), aunque luego no se escriba ninguno.
  bool logical = false;
  bool numerical = false;
  bool functions = false;
  for (const LearnedAction& action : domain.actions) {
    for (const rules::Examples* examples : {&action.logic, &action.numeric}) {
      for (const rules::Attribute& attribute : examples->attributes) {
        logical = logical || attribute.kind == rules::AttributeKind::atom ||
                  attribute.kind == rules::AttributeKind::relation;
        numerical = numerical || attribute.kind == rules::AttributeKind::value;
        functions = functions || (attribute.kind == rules::AttributeKind::value &&
                                  attribute.term.kind == datasets::AttributeKind::fluent);
      }
    }
  }

  std::ostringstream out;
  out << "(define (domain " << lower(name) << ")\n";
  out << "\t(:requirements :strips" << (domain.hierarchy.types.empty() ? "" : " :typing")
      << (options.numeric_fluents && functions ? " :numeric-fluents" : "") << ")\n";
  write_types(out, domain.hierarchy);
  std::vector<std::string> written;
  if (logical) {
    write_symbols(out, domain, false, written);
  }
  if (numerical) {
    write_symbols(out, domain, true, written);
  }

  for (const LearnedAction& action : domain.actions) {
    out << "\t(:action " << action.name << "\n";
    out << "\t\t:parameters (";
    const std::vector<std::string>& header = header_of(domain.hierarchy.actions, action.name);
    for (std::size_t p = 0; p < header.size(); ++p) {
      out << (p > 0 ? " " : "") << "?param_" << p << " - " << lower(header[p]);
    }
    out << ")\n";
    write_precondition(out, action.fused.precondition, options.equality);
    std::vector<rules::Literal> effect = action.fused.effect;
    for (const datasets::NumericEffect& numeric : action.effects) {
      if (std::optional<rules::Literal> written_effect = literal(numeric, action.argument_types)) {
        effect.push_back(std::move(*written_effect));
      }
    }
    write_effect(out, effect);
    out << "\t)\n";
  }
  out << ")";
  return out.str();
}

}  // namespace pddl
