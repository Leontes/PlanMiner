#include <gtest/gtest.h>

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "plantraces/trace.hpp"

namespace {

using nlohmann::json;
using plantraces::GroundAtom;
using plantraces::parse_atom;
using plantraces::parse_trace;
using plantraces::TraceError;

// Una traza de mundo cerrado con dos estados y una acción.
json closed_trace() {
  return json::parse(R"json({
    "format": "pddldomains-trace",
    "version": 1,
    "generator": "pddldomains 0.1.0",
    "domain": {"name": "blocks", "file": "domain.pddl"},
    "problem": {"name": "blocks-2", "file": "p01.pddl"},
    "world": "closed",
    "objects": {"a": "block", "b": ["either", "block", "thing"]},
    "states": [
      {"step": 0, "time": 0, "true": ["(clear a)", "(handempty)"], "unknown": ["(on a b)"],
       "values": {"(weight a)": 2.5}, "unknown_values": ["(weight b)"]},
      {"step": 1, "time": 1, "true": ["(holding a)"], "unknown": [], "values": {},
       "unknown_values": []}
    ],
    "actions": [{"step": 1, "time": 1, "name": "pick-up", "arguments": ["a"]}]
  })json");
}

// El mensaje de error al leer la traza, o "" si se lee bien.
std::string error_of(const json& trace) {
  try {
    parse_trace(trace.dump(), "t.json");
  } catch (const TraceError& error) {
    return error.what();
  }
  return "";
}

TEST(ParseAtom, ReadsNameAndArguments) {
  EXPECT_EQ(parse_atom("(on a b)"), (GroundAtom{"on", {"a", "b"}}));
  EXPECT_EQ(parse_atom("(handempty)"), (GroundAtom{"handempty", {}}));
  EXPECT_EQ(plantraces::to_string(parse_atom("(on a b)")), "(on a b)");
}

TEST(ParseAtom, RejectsMalformedAtoms) {
  for (const std::string text : {"on a b", "()", "( )", "(on (a) b)", "(", "(on a b"}) {
    EXPECT_THROW(parse_atom(text), TraceError) << text;
  }
}

TEST(ParseTrace, ReadsAClosedWorldTrace) {
  const plantraces::Trace trace = parse_trace(closed_trace().dump());

  EXPECT_EQ(trace.domain, "blocks");
  EXPECT_EQ(trace.problem, "blocks-2");
  EXPECT_EQ(trace.world, plantraces::World::closed);
  EXPECT_EQ(trace.objects.at("a"), std::vector<std::string>{"block"});
  EXPECT_EQ(trace.objects.at("b"), (std::vector<std::string>{"block", "thing"}));

  ASSERT_EQ(trace.states.size(), 2U);
  const plantraces::Observation& initial = trace.states[0];
  EXPECT_EQ(initial.truth({"clear", {"a"}}), true);
  EXPECT_EQ(initial.truth({"on", {"a", "b"}}), std::nullopt);
  // En mundo cerrado, lo que no se lista es falso.
  EXPECT_EQ(initial.truth({"on", {"b", "a"}}), false);
  EXPECT_EQ(initial.value({"weight", {"a"}}), 2.5);
  EXPECT_EQ(initial.value({"weight", {"b"}}), std::nullopt);
  EXPECT_TRUE(initial.unknown_values.contains({"weight", {"b"}}));

  ASSERT_EQ(trace.actions.size(), 1U);
  EXPECT_EQ(trace.actions[0].step, 1U);
  EXPECT_EQ(trace.actions[0].name, "pick-up");
  EXPECT_EQ(trace.actions[0].arguments, std::vector<std::string>{"a"});
}

TEST(ParseTrace, ReadsTheFalseAtomsOfAnOpenWorldTrace) {
  json trace = closed_trace();
  trace["world"] = "open";
  trace["states"][0]["false"] = {"(holding a)"};
  trace["states"][1]["false"] = json::array();

  const plantraces::Trace read = parse_trace(trace.dump());
  EXPECT_EQ(read.world, plantraces::World::open);
  EXPECT_TRUE(read.states[0].false_atoms.contains({"holding", {"a"}}));
  EXPECT_EQ(read.states[0].truth({"holding", {"a"}}), false);
}

TEST(ParseTrace, AMissingStateIsUnknown) {
  json trace = closed_trace();
  trace["states"][1] = {{"step", 1}, {"time", 1}, {"missing", true}};

  const plantraces::Observation state = parse_trace(trace.dump()).states[1];
  EXPECT_TRUE(state.missing);
  EXPECT_EQ(state.truth({"holding", {"a"}}), std::nullopt);
  EXPECT_EQ(state.truth({"on", {"b", "a"}}), std::nullopt);
}

TEST(ParseTrace, AcceptsRealTimesAndValues) {
  json trace = closed_trace();
  trace["states"][1]["time"] = 1.5;
  trace["actions"][0]["time"] = 0.5;
  trace["states"][1]["values"] = {{"(weight a)", 3}};

  const plantraces::Trace read = parse_trace(trace.dump());
  EXPECT_EQ(read.states[1].time, 1.5);
  EXPECT_EQ(read.actions[0].time, 0.5);
  EXPECT_EQ(read.states[1].value({"weight", {"a"}}), 3.0);
}

struct Rejection {
  std::string name;
  json trace;
  std::string message;
};

std::vector<Rejection> rejections() {
  std::vector<Rejection> cases;
  const auto add = [&cases](std::string name, const auto& change, std::string message) {
    json trace = closed_trace();
    change(trace);
    cases.push_back({std::move(name), std::move(trace), std::move(message)});
  };
  add("OtherFormat", [](json& t) { t["format"] = "otra-cosa"; }, "format: no es una traza");
  add("OtherVersion", [](json& t) { t["version"] = 2; }, "version: versión 2 no soportada");
  add("NoDomainName", [](json& t) { t["domain"].erase("name"); }, "domain: falta 'name'");
  add("BadWorld", [](json& t) { t["world"] = "both"; }, "world: se esperaba");
  add(
      "StepsOutOfOrder", [](json& t) { t["states"][1]["step"] = 2; },
      "states[1].step: se esperaba el paso 1");
  add(
      "TooManyActions", [](json& t) { t["actions"].push_back(t["actions"][0]); },
      "actions: hay 2 acciones y 2 estados");
  add(
      "ActionStep", [](json& t) { t["actions"][0]["step"] = 0; },
      "actions[0].step: se esperaba el paso 1");
  add(
      "MalformedAtom", [](json& t) { t["states"][0]["true"][0] = "clear a"; },
      "states[0].true[0]: átomo mal formado");
  add(
      "UndeclaredObjectInAtom", [](json& t) { t["states"][0]["true"][0] = "(clear z)"; },
      "states[0].true[0]: el objeto 'z' no está en objects");
  add(
      "UndeclaredObjectInAction", [](json& t) { t["actions"][0]["arguments"][0] = "z"; },
      "actions[0].arguments: el objeto 'z' no está en objects");
  add(
      "AtomWithTwoValues", [](json& t) { t["states"][0]["unknown"][0] = "(clear a)"; },
      "states[0].unknown[0]: el átomo (clear a) tiene más de un valor");
  add(
      "FalseInClosedWorld", [](json& t) { t["states"][0]["false"] = json::array(); },
      "states[0].false: una traza de mundo cerrado");
  add("OpenWorldWithoutFalse", [](json& t) { t["world"] = "open"; }, "states[0]: falta 'false'");
  add(
      "ValueAndUnknownValue", [](json& t) { t["states"][0]["unknown_values"][0] = "(weight a)"; },
      "states[0].unknown_values[0]: el fluente (weight a) también tiene valor");
  add(
      "ValueIsNotANumber", [](json& t) { t["states"][0]["values"]["(weight a)"] = "2.5"; },
      "states[0].values.(weight a): se esperaba un número");
  add(
      "MissingStateWithObservations", [](json& t) { t["states"][1]["missing"] = true; },
      "states[1]: un estado que falta");
  add(
      "NoStates",
      [](json& t) {
        t["states"] = json::array();
        t["actions"] = json::array();
      },
      "states: no hay estados");
  return cases;
}

class ParseTraceRejects : public testing::TestWithParam<Rejection> {};

TEST_P(ParseTraceRejects, WithAMessageThatSaysWhere) {
  const std::string error = error_of(GetParam().trace);
  ASSERT_FALSE(error.empty()) << "se ha aceptado";
  EXPECT_NE(error.find("t.json: " + GetParam().message), std::string::npos) << error;
}

INSTANTIATE_TEST_SUITE_P(Traces, ParseTraceRejects, testing::ValuesIn(rejections()),
                         [](const testing::TestParamInfo<Rejection>& rejection) {
                           return rejection.param.name;
                         });

TEST(ParseTrace, RejectsInvalidJson) {
  try {
    parse_trace("{\"format\": ", "t.json");
    FAIL() << "se ha aceptado";
  } catch (const TraceError& error) {
    EXPECT_NE(std::string{error.what()}.find("t.json: no es JSON válido"), std::string::npos)
        << error.what();
  }
}

TEST(ReadTrace, ReportsFilesThatCannotBeRead) {
  EXPECT_THROW(plantraces::read_trace("/no/existe.json"), TraceError);
}

}  // namespace
