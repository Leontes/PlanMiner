#include "pddl/domain.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <utility>

#include "datasets/effects.hpp"
#include "datasets/relations.hpp"
#include "datasets/table.hpp"
#include "inslv/inslv.hpp"
#include "pddl/simplify.hpp"
#include "plantraces/actions.hpp"
#include "plantraces/noise.hpp"
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
  options.exact_without_noise = false;
  options.contrast = false;
  options.drop_implied = false;
  options.rare_values = 0.0;
  options.rare_values_per_noise = false;
  options.rare_values_alpha = 0.0;
  options.smooth_fluents = false;
  options.refine_meta_states = false;
  options.smooth_atoms = false;
  options.smooth_values = false;
  options.exact_comparisons = false;
  options.noisy_accumulators = false;
  options.noisy_null_model = false;
  return options;
}

namespace {

// El ruido de las trazas, antes de suavizarlas: lo que cambia fuera de las acciones, en los átomos
// y los valores (plantraces::frame_changes) y solo en los valores (plantraces::value_changes).
struct Noise {
  double changes = 0.0;
  double values = 0.0;
};

// learn, con el ruido de las trazas antes de suavizarlas; si se han suavizado los átomos, el ruido
// que queda en cada columna; y los fluentes que ha suavizado plantraces::smooth_values.
LearnedDomain learn_domain(const std::vector<plantraces::Trace>& traces, const Noise& noise,
                           const plantraces::ColumnNoise* columns,
                           const std::set<std::string>& smoothed, const Options& options) {
  LearnedDomain domain;
  domain.hierarchy = plantraces::type_hierarchy(traces);
  const plantraces::Vocabulary vocabulary = plantraces::vocabulary(traces);
  const std::map<std::string, std::vector<plantraces::Transition>> actions =
      plantraces::group_by_action(traces);

  const bool noiseless = noise.changes == 0.0;
  // Con ruido en los valores, la probabilidad de que dos observaciones del mismo valor difieran.
  const double value_pairs = 1.0 - std::pow(1.0 - plantraces::value_noise(noise.values), 2.0);
  datasets::EffectOptions effect_options = options.effects;
  if (options.noisy_null_model) {
    effect_options.noise = value_pairs;
  }
  // El generador de INSLV, con la secuencia del rand() de glibc. INSLV lo siembra al empezar cada
  // aprendizaje, y la discretización sigue su secuencia: el original hacía lo mismo con el rand()
  // global. Antes, solo lo usa el suavizado de los fluentes.
  inslv::GeneradorDeAzar generator;
  const datasets::Random random = [&generator] { return generator(); };

  // Primero las tablas y los efectos numéricos de todas las acciones: las relaciones de cada una
  // dependen de qué fluentes cambian en el dominio. Ni la tabla ni la regresión usan números al
  // azar; el suavizado de los fluentes, sí.
  std::map<std::string, datasets::Table> tables;
  std::map<std::string, std::vector<datasets::FluentChange>> effects;
  datasets::DomainFluents fluents{
      datasets::accumulators(traces, options.noisy_accumulators ? value_pairs : 0.0), {}};
  for (const auto& [name, transitions] : actions) {
    datasets::Table table = datasets::build_table(vocabulary, transitions);
    if (options.aliasing) {
      datasets::hide_aliased_atoms(table, transitions);
    }
    if (options.smooth_fluents && !noiseless) {
      // Ni (total-cost) ni los acumuladores, ni lo que ya ha suavizado smooth_values.
      std::set<std::string> cumulative = fluents.accumulators;
      cumulative.insert("total-cost");
      cumulative.insert(smoothed.begin(), smoothed.end());
      datasets::smooth_fluents(table, random, {}, cumulative);
    }
    effects[name] = datasets::numeric_effects(table, effect_options);
    for (const datasets::FluentChange& change : effects[name]) {
      fluents.changing.insert(change.fluent.name);
    }
    tables.emplace(name, std::move(table));
  }
  if (options.noisy_accumulators && !noiseless) {
    // Los fluentes con algún efecto, y si todos son increase.
    std::map<std::string, bool> increasing;
    for (const auto& [name, changes] : effects) {
      for (const datasets::FluentChange& change : changes) {
        const bool increase =
            change.effect && change.effect->kind == datasets::EffectKind::increase;
        const auto [entry, inserted] = increasing.emplace(change.fluent.name, increase);
        entry->second = entry->second && increase;
      }
    }
    for (const auto& [fluent, increase] : increasing) {
      if (increase) {
        fluents.accumulators.insert(fluent);
      }
    }
  }

  rules::FusionOptions fusion = options.fusion;
  if (options.exact_without_noise && noiseless) {
    fusion.min_support = 1.0;
  }
  if (options.exact_comparisons && !noiseless) {
    fusion.min_comparison_support = 1.0;
  }
  if (options.refine_meta_states && !noiseless) {
    fusion.min_rule_support = 0.05;
    fusion.conflict_band = 0.1;
  }
  inslv::Opciones inslv_options;
  inslv_options.azar = &generator;
  for (const auto& [name, transitions] : actions) {
    LearnedAction action;
    action.name = name;
    for (const std::string& object : transitions.front().action->arguments) {
      action.argument_types.push_back(transitions.front().trace->objects.at(object).front());
    }
    const datasets::Table& table = tables.at(name);
    action.logic = rules::logic_examples(table);
    if (options.rare_values_alpha > 0.0 && !noiseless) {
      // El ruido de cada observación; con el suavizado de los átomos, el que queda en la columna.
      const double p = plantraces::observation_noise(noise.changes);
      const auto column_noise = [&](std::size_t attribute, std::size_t klass) {
        if (columns != nullptr) {
          const auto found =
              columns->find({action.name, action.logic.attributes[attribute].term.term});
          if (found != columns->end()) {
            return found->second[klass];
          }
        }
        return p;
      };
      rules::hide_rare_values_binomial(action.logic, column_noise, options.rare_values_alpha);
    } else if (options.rare_values > 0.0 && !noiseless) {
      const double threshold =
          options.rare_values_per_noise ? options.rare_values * noise.changes : options.rare_values;
      rules::hide_rare_values(action.logic, std::min(threshold, 0.5));
    }
    std::vector<rules::LegacyRule> rules = rules::legacy_rules(
        rules::learn(action.logic, inslv_options), action.logic, action.argument_types);

    const std::vector<datasets::FluentChange>& changes = effects.at(name);
    action.numeric = rules::numeric_examples(
        table, changes,
        datasets::numeric_relations(table, changes, options.relations, {fluents.accumulators, {}}),
        random, options.examples);
    const std::vector<rules::LegacyRule> numeric = rules::legacy_rules(
        rules::learn(action.numeric, inslv_options), action.numeric, action.argument_types);
    rules.insert(rules.end(), numeric.begin(), numeric.end());
    if (options.contrast && noiseless) {
      rules::LegacyRule contrast = rules::contrast_rule(
          datasets::candidate_relations(table, changes, options.relations, fluents), action.numeric,
          action.argument_types, traces, fusion.min_support);
      if (!contrast.conditions.empty()) {
        rules.push_back(std::move(contrast));
      }
    }

    action.fused = rules::fuse(
        rules, header_of(domain.hierarchy.actions, name),
        rules::evidence({&action.logic, &action.numeric}, action.argument_types), fusion);
    if (options.drop_implied) {
      action.fused.precondition = drop_implied(action.fused.precondition, transitions, traces);
    }
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

// Si hay algo que suavizar en las trazas.
bool smooths(const Noise& noise, const Options& options) {
  return (options.smooth_atoms && noise.changes > 0.0) ||
         (options.smooth_values && noise.values > 0.0);
}

// Con el suavizado de los átomos o de los fluentes: las trazas se suavizan en su sitio.
LearnedDomain learn_smoothed(std::vector<plantraces::Trace>& traces, const Noise& noise,
                             const Options& options) {
  std::set<std::string> smoothed;
  if (options.smooth_values && noise.values > 0.0) {
    smoothed = plantraces::smooth_values(traces, plantraces::value_noise(noise.values));
  }
  if (options.smooth_atoms && noise.changes > 0.0) {
    const plantraces::ColumnNoise columns =
        plantraces::smooth_atoms(traces, plantraces::observation_noise(noise.changes));
    return learn_domain(traces, noise, &columns, smoothed, options);
  }
  return learn_domain(traces, noise, nullptr, smoothed, options);
}

}  // namespace

LearnedDomain learn(const std::vector<plantraces::Trace>& traces, const Options& options) {
  const Noise noise{plantraces::frame_changes(traces), plantraces::value_changes(traces)};
  if (smooths(noise, options)) {
    std::vector<plantraces::Trace> copy = traces;
    return learn_smoothed(copy, noise, options);
  }
  return learn_domain(traces, noise, nullptr, {}, options);
}

LearnedDomain learn(std::vector<plantraces::Trace>&& traces, const Options& options) {
  const Noise noise{plantraces::frame_changes(traces), plantraces::value_changes(traces)};
  if (smooths(noise, options)) {
    return learn_smoothed(traces, noise, options);
  }
  return learn_domain(traces, noise, nullptr, {}, options);
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
