#include <gtest/gtest.h>

#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <vector>

#include "plantraces/actions.hpp"

namespace {

using nlohmann::json;
using plantraces::LiftedAtom;

// Una traza de zenotravel con un vuelo de una ciudad a sí misma y otro entre dos ciudades.
plantraces::Trace zenotravel() {
  return plantraces::parse_trace(json::parse(R"json({
    "format": "pddldomains-trace",
    "version": 1,
    "domain": {"name": "zenotravel"},
    "problem": {"name": "p"},
    "world": "closed",
    "objects": {"p1": "aircraft", "ana": "person", "c0": "city", "c1": "city"},
    "states": [
      {"step": 0, "time": 0, "true": ["(located p1 c0)", "(located ana c0)"], "unknown": [],
       "values": {"(fuel p1)": 10, "(distance c0 c1)": 4}, "unknown_values": []},
      {"step": 1, "time": 1, "true": ["(located p1 c0)", "(located ana c0)"], "unknown": [],
       "values": {"(fuel p1)": 10, "(distance c0 c1)": 4}, "unknown_values": []},
      {"step": 2, "time": 2, "true": ["(located p1 c1)", "(located ana c0)"], "unknown": [],
       "values": {"(fuel p1)": 6}, "unknown_values": ["(distance c0 c1)"]}
    ],
    "actions": [
      {"step": 1, "time": 1, "name": "fly", "arguments": ["p1", "c0", "c0"]},
      {"step": 2, "time": 2, "name": "fly", "arguments": ["p1", "c0", "c1"]}
    ]
  })json")
                                     .dump());
}

TEST(Vocabulary, ObservesArityAndArgumentTypes) {
  const plantraces::Vocabulary vocabulary = plantraces::vocabulary({zenotravel()});

  ASSERT_TRUE(vocabulary.predicates.contains("located"));
  const plantraces::Signature& located = vocabulary.predicates.at("located");
  EXPECT_EQ(located.arity, 2U);
  EXPECT_EQ(located.types[0], (std::set<std::string>{"aircraft", "person"}));
  EXPECT_EQ(located.types[1], std::set<std::string>{"city"});

  // Los fluentes se observan en los valores y en los desconocidos.
  EXPECT_EQ(vocabulary.fluents.size(), 2U);
  EXPECT_EQ(vocabulary.fluents.at("distance").types[1], std::set<std::string>{"city"});
}

TEST(Vocabulary, RejectsSymbolsWithTwoArities) {
  const plantraces::Trace trace = plantraces::parse_trace(R"json({
    "format": "pddldomains-trace", "version": 1, "domain": {"name": "d"}, "problem": {"name": "p"},
    "world": "closed", "objects": {"a": "object"},
    "states": [{"step": 0, "time": 0, "true": ["(p a)", "(p)"], "unknown": [], "values": {},
                "unknown_values": []}],
    "actions": []
  })json");
  EXPECT_THROW(plantraces::vocabulary({trace}), plantraces::TraceError);
}

TEST(GroupByAction, KeepsTheOrderOfTracesAndSteps) {
  const std::vector<plantraces::Trace> traces{zenotravel(), zenotravel()};
  const auto actions = plantraces::group_by_action(traces);

  ASSERT_EQ(actions.size(), 1U);
  const std::vector<plantraces::Transition>& fly = actions.at("fly");
  ASSERT_EQ(fly.size(), 4U);
  EXPECT_EQ(fly[0].trace, &traces[0]);
  EXPECT_EQ(fly[1].action, &traces[0].actions[1]);
  EXPECT_EQ(fly[1].before, &traces[0].states[1]);
  EXPECT_EQ(fly[1].after, &traces[0].states[2]);
  EXPECT_EQ(fly[2].trace, &traces[1]);
}

TEST(Expressible, OnlyAtomsOverTheParametersThatFitTheTypes) {
  const std::vector<plantraces::Trace> traces{zenotravel()};
  const plantraces::Vocabulary vocabulary = plantraces::vocabulary(traces);
  const plantraces::Transition flight = plantraces::group_by_action(traces).at("fly")[1];

  std::vector<std::string> atoms;
  for (const plantraces::Expressible& atom :
       plantraces::expressible(vocabulary.predicates, flight)) {
    atoms.push_back(plantraces::to_string(atom.ground) + " " + plantraces::to_string(atom.lifted));
  }
  // located(aircraft|person, city): ana no es argumento de la acción, y una ciudad no puede ir en
  // el primer argumento.
  EXPECT_EQ(atoms, (std::vector<std::string>{"(located p1 c0) (located ?0 ?1)",
                                             "(located p1 c1) (located ?0 ?2)"}));
}

TEST(Expressible, AnObjectIsAlwaysItsFirstParameter) {
  const std::vector<plantraces::Trace> traces{zenotravel()};
  const plantraces::Vocabulary vocabulary = plantraces::vocabulary(traces);
  // fly p1 c0 c0: c0 es ?1, nunca ?2.
  const plantraces::Transition loop = plantraces::group_by_action(traces).at("fly")[0];

  std::vector<LiftedAtom> fluents;
  for (const plantraces::Expressible& fluent : plantraces::expressible(vocabulary.fluents, loop)) {
    fluents.push_back(fluent.lifted);
  }
  EXPECT_EQ(fluents, (std::vector<LiftedAtom>{{"distance", {1, 1}}, {"fuel", {0}}}));
}

// Sin ruido, una acción solo cambia lo de sus argumentos (y lo que no tiene argumentos).
TEST(FrameChanges, NoneWithoutNoise) { EXPECT_EQ(plantraces::frame_changes({zenotravel()}), 0.0); }

// fly p1 c0 c1: (located bob c2) y (fuel p2) cambian sin ser de la acción, y (handempty) no
// cuenta. De los 3 átomos y fluentes con argumentos fuera de la acción, cambian 2.
TEST(FrameChanges, WhatChangesOutsideTheAction) {
  const plantraces::Trace trace = plantraces::parse_trace(json::parse(R"json({
    "format": "pddldomains-trace", "version": 1, "domain": {"name": "zenotravel"},
    "problem": {"name": "p"}, "world": "closed",
    "objects": {"p1": "aircraft", "p2": "aircraft", "bob": "person", "c0": "city", "c1": "city",
                "c2": "city"},
    "states": [
      {"step": 0, "time": 0, "true": ["(located p1 c0)", "(located bob c2)", "(handempty)"],
       "unknown": [], "values": {"(fuel p1)": 10, "(fuel p2)": 5, "(capacity p2)": 9},
       "unknown_values": []},
      {"step": 1, "time": 1, "true": ["(located p1 c1)"], "unknown": [],
       "values": {"(fuel p1)": 6, "(fuel p2)": 4, "(capacity p2)": 9}, "unknown_values": []}
    ],
    "actions": [{"step": 1, "time": 1, "name": "fly", "arguments": ["p1", "c0", "c1"]}]
  })json")
                                                              .dump());
  EXPECT_DOUBLE_EQ(plantraces::frame_changes({trace}), 2.0 / 3.0);
}

TEST(LiftedAtom, Text) {
  EXPECT_EQ(plantraces::to_string(LiftedAtom{"on", {0, 1}}), "(on ?0 ?1)");
  EXPECT_EQ(plantraces::to_string(LiftedAtom{"handempty", {}}), "(handempty)");
}

}  // namespace
