// La simplificación de las precondiciones con los estados de las trazas.

#include <gtest/gtest.h>

#include <ostream>
#include <string>
#include <vector>

#include "pddl/simplify.hpp"

namespace rules {

void PrintTo(const Literal& literal, std::ostream* out) {
  *out << "[" << literal.attribute << " = " << literal.value << "]";
}

}  // namespace rules

namespace {

using rules::Literal;

// visit-all con un grafo simétrico a - b - c: el robot va de a a b y de b a c, y visita donde está.
std::vector<plantraces::Trace> visit_all() {
  return {plantraces::parse_trace(R"json({
    "format": "pddldomains-trace", "version": 1, "domain": {"name": "visit-all"},
    "problem": {"name": "p"}, "world": "closed",
    "objects": {"a": "place", "b": "place", "c": "place"},
    "states": [
      {"step": 0, "time": 0, "unknown": [], "values": {}, "unknown_values": [],
       "true": ["(at-robot a)", "(visited a)", "(connected a b)", "(connected b a)",
                "(connected b c)", "(connected c b)"]},
      {"step": 1, "time": 1, "unknown": [], "values": {}, "unknown_values": [],
       "true": ["(at-robot b)", "(visited a)", "(visited b)", "(connected a b)",
                "(connected b a)", "(connected b c)", "(connected c b)"]},
      {"step": 2, "time": 2, "unknown": [], "values": {}, "unknown_values": [],
       "true": ["(at-robot c)", "(visited a)", "(visited b)", "(visited c)", "(connected a b)",
                "(connected b a)", "(connected b c)", "(connected c b)"]}
    ],
    "actions": [
      {"step": 1, "time": 1, "name": "move", "arguments": ["a", "b"]},
      {"step": 2, "time": 2, "name": "move", "arguments": ["b", "c"]}
    ]
  })json")};
}

// De (connected ?0 ?1) y su inversa se queda la de los parámetros en orden; (visited ?0), que solo
// implica (at-robot ?0), y el átomo falso, que no se escribe, se quedan.
TEST(DropImplied, EquivalentAtomsOfTheSameRelation) {
  const std::vector<plantraces::Trace> traces = visit_all();
  const auto actions = plantraces::group_by_action(traces);
  const std::vector<Literal> precondition = {
      {"( connected ?param_1 ~ ?place ?param_0 ~ ?place ) ", "True"},
      {"( at-robot ?param_0 ~ ?place ) ", "True"},
      {"( visited ?param_1 ~ ?place ) ", "False"},
      {"( connected ?param_0 ~ ?place ?param_1 ~ ?place ) ", "True"},
      {"( visited ?param_0 ~ ?place ) ", "True"},
  };
  EXPECT_EQ(pddl::drop_implied(precondition, actions.at("move"), traces),
            (std::vector<Literal>{
                {"( at-robot ?param_0 ~ ?place ) ", "True"},
                {"( visited ?param_1 ~ ?place ) ", "False"},
                {"( connected ?param_0 ~ ?place ?param_1 ~ ?place ) ", "True"},
                {"( visited ?param_0 ~ ?place ) ", "True"},
            }));
}

// debark en zenotravel: (in ?0 ?1) implica que onboard es al menos 1, y esa comparación sobra. La
// del combustible no la implica: en el último estado, ana está en p1 con poco combustible.
TEST(DropImplied, ComparisonsImpliedByTheRest) {
  const std::vector<plantraces::Trace> traces = {plantraces::parse_trace(R"json({
    "format": "pddldomains-trace", "version": 1, "domain": {"name": "zenotravel"},
    "problem": {"name": "p"}, "world": "closed", "objects": {"ana": "person", "p1": "aircraft"},
    "states": [
      {"step": 0, "time": 0, "unknown": [], "unknown_values": [], "true": ["(in ana p1)"],
       "values": {"(onboard p1)": 1, "(fuel p1)": 5}},
      {"step": 1, "time": 1, "unknown": [], "unknown_values": [], "true": [],
       "values": {"(onboard p1)": 0, "(fuel p1)": 5}},
      {"step": 2, "time": 2, "unknown": [], "unknown_values": [], "true": ["(in ana p1)"],
       "values": {"(onboard p1)": 1, "(fuel p1)": 1}}
    ],
    "actions": [
      {"step": 1, "time": 1, "name": "debark", "arguments": ["ana", "p1"]},
      {"step": 2, "time": 2, "name": "board", "arguments": ["ana", "p1"]}
    ]
  })json")};
  const auto actions = plantraces::group_by_action(traces);
  const std::vector<Literal> precondition = {
      {"( in ?param_0 ~ ?person ?param_1 ~ ?aircraft ) ", "True"},
      {"opGET - ( onboard ?param_1 ~ ?aircraft ) 1", "True"},
      {"opGT - ( fuel ?param_1 ~ ?aircraft ) ( * ( onboard ?param_1 ~ ?aircraft ) 2 ) ", "True"},
  };
  EXPECT_EQ(pddl::drop_implied(precondition, actions.at("debark"), traces),
            (std::vector<Literal>{
                {"( in ?param_0 ~ ?person ?param_1 ~ ?aircraft ) ", "True"},
                {"opGT - ( fuel ?param_1 ~ ?aircraft ) ( * ( onboard ?param_1 ~ ?aircraft ) 2 ) ",
                 "True"},
            }));
}

}  // namespace
