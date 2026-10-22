#include "plantraces/trace.hpp"

#include <cstddef>
#include <fstream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <utility>

namespace plantraces {

namespace {

using Json = nlohmann::json;

constexpr std::string_view trace_format = "pddldomains-trace";
constexpr int trace_version = 1;

std::optional<GroundAtom> try_parse_atom(std::string_view text) {
  if (text.size() < 2 || text.front() != '(' || text.back() != ')') {
    return std::nullopt;
  }
  std::vector<std::string> tokens;
  std::string token;
  for (const char c : text.substr(1, text.size() - 2)) {
    if (c == '(' || c == ')') {
      return std::nullopt;
    }
    if (c == ' ') {
      if (!token.empty()) {
        tokens.push_back(std::move(token));
        token.clear();
      }
    } else {
      token += c;
    }
  }
  if (!token.empty()) {
    tokens.push_back(std::move(token));
  }
  if (tokens.empty()) {
    return std::nullopt;
  }
  GroundAtom atom{std::move(tokens.front()), {}};
  atom.arguments.assign(std::make_move_iterator(tokens.begin() + 1),
                        std::make_move_iterator(tokens.end()));
  return atom;
}

std::string at(const std::string& path, std::size_t index) {
  return path + "[" + std::to_string(index) + "]";
}

std::string at(const std::string& path, std::string_view key) {
  return path.empty() ? std::string{key} : path + "." + std::string{key};
}

// Lee la traza y comprueba su coherencia. Los errores dicen dónde están: la fuente y la ruta
// dentro del JSON (states[3].true[2]).
class Reader {
 public:
  explicit Reader(std::string_view source) : source_(source) {}

  Trace read(const Json& root) {
    Trace trace;
    check_format(root);
    trace.domain = text(member(root, "", "domain"), "domain", "name");
    trace.problem = text(member(root, "", "problem"), "problem", "name");
    trace.objects = objects(member(root, "", "objects"));
    trace.world = world(member(root, "", "world"));

    const Json& states = list(member(root, "", "states"), "states");
    for (std::size_t i = 0; i < states.size(); ++i) {
      trace.states.push_back(observation(states[i], at("states", i), i, trace));
    }
    if (trace.states.empty()) {
      fail("states", "no hay estados");
    }

    const Json& actions = list(member(root, "", "actions"), "actions");
    if (actions.size() + 1 != trace.states.size()) {
      fail("actions", "hay " + std::to_string(actions.size()) + " acciones y " +
                          std::to_string(trace.states.size()) +
                          " estados: tiene que haber una acción por estado, salvo el inicial");
    }
    for (std::size_t i = 0; i < actions.size(); ++i) {
      trace.actions.push_back(action(actions[i], at("actions", i), i + 1, trace));
    }
    return trace;
  }

 private:
  [[noreturn]] void fail(const std::string& path, const std::string& message) const {
    throw TraceError(source_ + ": " + (path.empty() ? "" : path + ": ") + message);
  }

  const Json& member(const Json& object, const std::string& path, std::string_view key) const {
    if (!object.is_object()) {
      fail(path, "se esperaba un objeto");
    }
    const auto found = object.find(key);
    if (found == object.end()) {
      fail(path, "falta '" + std::string{key} + "'");
    }
    return *found;
  }

  std::string string(const Json& value, const std::string& path) const {
    if (!value.is_string()) {
      fail(path, "se esperaba un texto");
    }
    return value.get<std::string>();
  }

  std::string text(const Json& object, const std::string& path, std::string_view key) const {
    return string(member(object, path, key), at(path, key));
  }

  double number(const Json& value, const std::string& path) const {
    if (!value.is_number()) {
      fail(path, "se esperaba un número");
    }
    return value.get<double>();
  }

  std::size_t step(const Json& value, const std::string& path, std::size_t expected) const {
    if (!value.is_number_unsigned() || value.get<std::size_t>() != expected) {
      fail(path, "se esperaba el paso " + std::to_string(expected) + " y es " + value.dump());
    }
    return expected;
  }

  const Json& list(const Json& value, const std::string& path) const {
    if (!value.is_array()) {
      fail(path, "se esperaba una lista");
    }
    return value;
  }

  void check_format(const Json& root) const {
    const Json& format = member(root, "", "format");
    if (!format.is_string() || format.get<std::string>() != trace_format) {
      fail("format", "no es una traza de PDDLDomains (" + format.dump() + ")");
    }
    const Json& version = member(root, "", "version");
    if (!version.is_number_integer() || version.get<int>() != trace_version) {
      fail("version", "versión " + version.dump() + " no soportada (se lee la " +
                          std::to_string(trace_version) + ")");
    }
  }

  std::map<std::string, std::vector<std::string>> objects(const Json& value) const {
    if (!value.is_object()) {
      fail("objects", "se esperaba un objeto");
    }
    std::map<std::string, std::vector<std::string>> result;
    for (const auto& [name, type] : value.items()) {
      const std::string path = at("objects", name);
      if (type.is_string()) {
        result[name] = {type.get<std::string>()};
        continue;
      }
      // ["either", "t1", "t2", ...]
      if (!type.is_array() || type.size() < 2 || type[0] != "either") {
        fail(path, "se esperaba un tipo o [\"either\", tipo, ...]");
      }
      for (std::size_t i = 1; i < type.size(); ++i) {
        result[name].push_back(string(type[i], at(path, i)));
      }
    }
    return result;
  }

  World world(const Json& value) const {
    if (value == "closed") {
      return World::closed;
    }
    if (value == "open") {
      return World::open;
    }
    fail("world", "se esperaba \"closed\" u \"open\" y es " + value.dump());
  }

  void check_objects(const std::vector<std::string>& arguments, const std::string& path,
                     const Trace& trace) const {
    for (const std::string& argument : arguments) {
      if (!trace.objects.contains(argument)) {
        fail(path, "el objeto '" + argument + "' no está en objects");
      }
    }
  }

  GroundAtom atom(const Json& value, const std::string& path, const Trace& trace) const {
    const std::string written = string(value, path);
    std::optional<GroundAtom> parsed = try_parse_atom(written);
    if (!parsed) {
      fail(path, "átomo mal formado: '" + written + "'");
    }
    check_objects(parsed->arguments, path, trace);
    return std::move(*parsed);
  }

  // Los átomos de una de las listas de un estado. Ninguno puede estar ya en otra.
  void atoms(const Json& state, const std::string& path, std::string_view key,
             std::set<GroundAtom>& into, const Observation& observation, const Trace& trace) const {
    const std::string list_path = at(path, key);
    const Json& values = list(member(state, path, key), list_path);
    for (std::size_t i = 0; i < values.size(); ++i) {
      GroundAtom parsed = atom(values[i], at(list_path, i), trace);
      if (observation.true_atoms.contains(parsed) || observation.false_atoms.contains(parsed) ||
          observation.unknown_atoms.contains(parsed)) {
        fail(at(list_path, i), "el átomo " + to_string(parsed) + " tiene más de un valor");
      }
      into.insert(std::move(parsed));
    }
  }

  Observation observation(const Json& state, const std::string& path, std::size_t index,
                          const Trace& trace) const {
    Observation result;
    result.step = step(member(state, path, "step"), at(path, "step"), index);
    result.time = number(member(state, path, "time"), at(path, "time"));

    if (state.contains("missing")) {
      if (state["missing"] != true) {
        fail(at(path, "missing"), "se esperaba true");
      }
      if (state.size() != 3) {
        fail(path, "un estado que falta solo tiene step, time y missing");
      }
      result.missing = true;
      return result;
    }

    atoms(state, path, "true", result.true_atoms, result, trace);
    if (trace.world == World::open) {
      atoms(state, path, "false", result.false_atoms, result, trace);
    } else if (state.contains("false")) {
      fail(at(path, "false"), "una traza de mundo cerrado no lista los átomos falsos");
    }
    atoms(state, path, "unknown", result.unknown_atoms, result, trace);

    const std::string values_path = at(path, "values");
    const Json& values = member(state, path, "values");
    if (!values.is_object()) {
      fail(values_path, "se esperaba un objeto");
    }
    for (const auto& [term, value] : values.items()) {
      const std::string term_path = at(values_path, term);
      result.values.emplace(atom(term, term_path, trace), number(value, term_path));
    }

    const std::string unknown_path = at(path, "unknown_values");
    const Json& unknown = list(member(state, path, "unknown_values"), unknown_path);
    for (std::size_t i = 0; i < unknown.size(); ++i) {
      GroundAtom term = atom(unknown[i], at(unknown_path, i), trace);
      if (result.values.contains(term)) {
        fail(at(unknown_path, i), "el fluente " + to_string(term) + " también tiene valor");
      }
      result.unknown_values.insert(std::move(term));
    }
    return result;
  }

  GroundAction action(const Json& value, const std::string& path, std::size_t index,
                      const Trace& trace) const {
    GroundAction result;
    result.step = step(member(value, path, "step"), at(path, "step"), index);
    result.time = number(member(value, path, "time"), at(path, "time"));
    result.name = text(value, path, "name");
    const std::string arguments_path = at(path, "arguments");
    const Json& arguments = list(member(value, path, "arguments"), arguments_path);
    for (std::size_t i = 0; i < arguments.size(); ++i) {
      result.arguments.push_back(string(arguments[i], at(arguments_path, i)));
    }
    check_objects(result.arguments, arguments_path, trace);
    return result;
  }

  std::string source_;
};

}  // namespace

std::optional<bool> Observation::truth(const GroundAtom& atom) const {
  if (missing || unknown_atoms.contains(atom)) {
    return std::nullopt;
  }
  return true_atoms.contains(atom);
}

std::optional<double> Observation::value(const GroundAtom& term) const {
  const auto found = values.find(term);
  if (found == values.end()) {
    return std::nullopt;
  }
  return found->second;
}

GroundAtom parse_atom(std::string_view text) {
  std::optional<GroundAtom> atom = try_parse_atom(text);
  if (!atom) {
    throw TraceError("átomo mal formado: '" + std::string{text} + "'");
  }
  return std::move(*atom);
}

std::string to_string(const GroundAtom& atom) {
  std::string text = "(" + atom.name;
  for (const std::string& argument : atom.arguments) {
    text += " " + argument;
  }
  return text + ")";
}

Trace parse_trace(std::string_view json, std::string_view source) {
  Json root;
  try {
    root = Json::parse(json);
  } catch (const Json::parse_error& error) {
    throw TraceError(std::string{source} + ": no es JSON válido: " + error.what());
  }
  return Reader(source).read(root);
}

Trace read_trace(const std::filesystem::path& file) {
  std::ifstream in(file, std::ios::binary);
  if (!in) {
    throw TraceError(file.string() + ": no se puede leer");
  }
  std::ostringstream content;
  content << in.rdbuf();
  return parse_trace(content.str(), file.string());
}

}  // namespace plantraces
