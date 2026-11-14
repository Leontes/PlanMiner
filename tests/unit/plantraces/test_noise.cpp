#include <gtest/gtest.h>

#include <string>

#include "plantraces/noise.hpp"
#include "plantraces/trace.hpp"

namespace {

// Un vuelo fly p1 c0 c1, con lo que no es de la acción en los dos estados: (located bob c2),
// (fuel p2) y (capacity p2), y (handempty) en el de antes.
plantraces::Trace flight(const std::string& after) {
  return plantraces::parse_trace(R"json({
    "format": "pddldomains-trace", "version": 1, "domain": {"name": "zenotravel"},
    "problem": {"name": "p"}, "world": "closed",
    "objects": {"p1": "aircraft", "p2": "aircraft", "bob": "person", "c0": "city", "c1": "city",
                "c2": "city"},
    "states": [
      {"step": 0, "time": 0, "true": ["(located p1 c0)", "(located bob c2)", "(handempty)"],
       "unknown": [], "values": {"(fuel p1)": 10, "(fuel p2)": 5, "(capacity p2)": 9},
       "unknown_values": []},
      )json" + after + R"json(
    ],
    "actions": [{"step": 1, "time": 1, "name": "fly", "arguments": ["p1", "c0", "c1"]}]
  })json");
}

// Sin ruido, una acción solo cambia lo de sus argumentos (y lo que no tiene argumentos: aquí,
// (handempty)).
TEST(FrameChanges, NoneWithoutNoise) {
  EXPECT_EQ(plantraces::frame_changes({flight(R"json(
      {"step": 1, "time": 1, "true": ["(located p1 c1)", "(located bob c2)"], "unknown": [],
       "values": {"(fuel p1)": 6, "(fuel p2)": 5, "(capacity p2)": 9}, "unknown_values": []}
  )json")}),
            0.0);
}

// (located bob c2) y (fuel p2) cambian sin ser de la acción, y (handempty) no cuenta. De los 3
// átomos y fluentes con argumentos fuera de la acción, cambian 2.
TEST(FrameChanges, WhatChangesOutsideTheAction) {
  EXPECT_DOUBLE_EQ(plantraces::frame_changes({flight(R"json(
      {"step": 1, "time": 1, "true": ["(located p1 c1)"], "unknown": [],
       "values": {"(fuel p1)": 6, "(fuel p2)": 4, "(capacity p2)": 9}, "unknown_values": []}
  )json")}),
                   2.0 / 3.0);
}

// c = 2p(1 - p): con un 5 % de ruido en cada observación, c = 0.095.
TEST(ObservationNoise, TheInverseOfTheChanges) {
  EXPECT_EQ(plantraces::observation_noise(0.0), 0.0);
  EXPECT_NEAR(plantraces::observation_noise(2 * 0.05 * 0.95), 0.05, 1e-12);
  EXPECT_NEAR(plantraces::observation_noise(2 * 0.2 * 0.8), 0.2, 1e-12);
  EXPECT_EQ(plantraces::observation_noise(0.5), 0.5);
  EXPECT_EQ(plantraces::observation_noise(0.7), 0.5);
}

// P(X > m) <= alpha con X ~ Binomial(n, p). Con n = 2 y p = 0.5, P(X <= 0) = 0.25 y P(X <= 1) =
// 0.75. Con p = 0.05 y alpha = 1e-4, el umbral en proporción baja con las filas: 30 % con 20, 15 %
// con 100 y 7.7 % con 1000.
TEST(BinomialBound, TheTailOfTheBinomial) {
  EXPECT_EQ(plantraces::binomial_bound(2, 0.5, 0.3), 1U);
  EXPECT_EQ(plantraces::binomial_bound(2, 0.5, 0.2), 2U);
  EXPECT_EQ(plantraces::binomial_bound(20, 0.05, 1e-4), 6U);
  EXPECT_EQ(plantraces::binomial_bound(100, 0.05, 1e-4), 15U);
  EXPECT_EQ(plantraces::binomial_bound(100, 0.05, 1e-3), 13U);
  EXPECT_EQ(plantraces::binomial_bound(1000, 0.05, 1e-4), 77U);
  EXPECT_EQ(plantraces::binomial_bound(100, 0.0, 1e-4), 0U);
  EXPECT_EQ(plantraces::binomial_bound(0, 0.05, 1e-4), 0U);
}

}  // namespace
