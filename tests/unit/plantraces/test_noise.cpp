#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <utility>
#include <vector>

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

// Un robot r que va y viene entre las habitaciones a y b (move r a b, move r b a...). (at r a) y
// (at r b) cambian con cada move; (lit a) y (lit b), nunca, y en el estado 2 el ruido ha borrado
// (lit a). c y e no son de ninguna acción: (lit c) sale cierta 3 veces y falsa 2, y (lit e),
// cierta solo en el estado 2. (lit d) solo se conoce en el último estado. (handempty) no tiene
// argumentos.
plantraces::Trace shuttle() {
  return plantraces::parse_trace(R"json({
    "format": "pddldomains-trace", "version": 1, "domain": {"name": "shuttle"},
    "problem": {"name": "p"}, "world": "closed",
    "objects": {"r": "robot", "a": "room", "b": "room", "c": "room", "d": "room", "e": "room"},
    "states": [
      {"step": 0, "time": 0, "true": ["(at r a)", "(lit a)", "(lit b)", "(lit c)", "(handempty)"],
       "unknown": ["(lit d)"], "values": {}, "unknown_values": []},
      {"step": 1, "time": 1, "true": ["(at r b)", "(lit a)", "(lit b)", "(lit c)"],
       "unknown": ["(lit d)"], "values": {}, "unknown_values": []},
      {"step": 2, "time": 2, "true": ["(at r a)", "(lit b)", "(lit e)"],
       "unknown": ["(lit d)"], "values": {}, "unknown_values": []},
      {"step": 3, "time": 3, "true": ["(at r b)", "(lit a)", "(lit b)"],
       "unknown": ["(lit d)"], "values": {}, "unknown_values": []},
      {"step": 4, "time": 4, "true": ["(at r a)", "(lit a)", "(lit b)", "(lit c)", "(lit d)"],
       "unknown": [], "values": {}, "unknown_values": []}
    ],
    "actions": [
      {"step": 1, "time": 1, "name": "move", "arguments": ["r", "a", "b"]},
      {"step": 2, "time": 2, "name": "move", "arguments": ["r", "b", "a"]},
      {"step": 3, "time": 3, "name": "move", "arguments": ["r", "a", "b"]},
      {"step": 4, "time": 4, "name": "move", "arguments": ["r", "b", "a"]}
    ]
  })json");
}

std::optional<bool> truth(const plantraces::Trace& trace, std::size_t state,
                          const std::string& atom) {
  return trace.states[state].truth(plantraces::parse_atom(atom));
}

// Con p = 0.1, entre dos observaciones el ruido cambia 2p(1 - p) = 0.18. (lit ?1) y (lit ?2)
// cambian en 1 de 4 aplicaciones de move: lo explica el ruido (con alpha 0.3, hasta 1), y move no
// los corta. (at ?0 ?1) y (at ?0 ?2) cambian en las 4: son efectos.
TEST(SmoothAtoms, EachAtomBetweenTheActionsThatChangeIt) {
  std::vector<plantraces::Trace> traces{shuttle()};
  plantraces::smooth_atoms(traces, 0.1);
  const plantraces::Trace& trace = traces.front();

  // (lit a): cierta 4 veces y falsa 1 en la traza, que es un solo tramo; la mayoría la corrige.
  EXPECT_EQ(truth(trace, 2, "(lit a)"), true);
  // (lit e): falsa 4 veces y cierta 1. En mundo cerrado, los falsos no se listan.
  EXPECT_EQ(truth(trace, 2, "(lit e)"), false);
  EXPECT_TRUE(trace.states[2].false_atoms.empty());
  // (lit c): 3 contra 2, sin una mayoría clara: desconocida.
  for (std::size_t state = 0; state < 5; ++state) {
    EXPECT_EQ(truth(trace, state, "(lit c)"), std::nullopt) << state;
  }
  // (lit d): una sola observación, como está.
  EXPECT_EQ(truth(trace, 4, "(lit d)"), true);
  // (at r a): cada move la corta, y cada tramo tiene una sola observación. Sin los cortes, con 3
  // contra 2, sería desconocida.
  EXPECT_EQ(truth(trace, 1, "(at r a)"), false);
  EXPECT_EQ(truth(trace, 2, "(at r a)"), true);
  // (handempty) no se suaviza.
  EXPECT_EQ(truth(trace, 0, "(handempty)"), true);
}

// El ruido que queda en las columnas: en (lit ?1) de move, antes, las observaciones de (lit a) en
// los estados 0 y 2 y las de (lit b) en 1 y 3, con una mayoría de 3 y de 5: 1 / (1 + 9^3) y
// 1 / (1 + 9^5); después, en 1 y 3 y en 2 y 4. En (at ?0 ?1), las de una sola observación: p.
TEST(SmoothAtoms, TheNoiseThatRemainsInEachColumn) {
  std::vector<plantraces::Trace> traces{shuttle()};
  const plantraces::ColumnNoise noise = plantraces::smooth_atoms(traces, 0.1);

  const double three = 1.0 / (1.0 + 729.0);
  const double five = 1.0 / (1.0 + 59049.0);
  const std::array<double, 2> lit = noise.at({"move", {"lit", {1}}});
  EXPECT_NEAR(lit[0], (three + five) / 2.0, 1e-15);
  EXPECT_NEAR(lit[1], (three + five) / 2.0, 1e-15);
  EXPECT_DOUBLE_EQ(noise.at({"move", {"at", {0, 1}}})[0], 0.1);
  EXPECT_DOUBLE_EQ(noise.at({"move", {"at", {0, 2}}})[1], 0.1);
  // (lit c) no es de ninguna acción, y (handempty) no se suaviza.
  EXPECT_EQ(noise.size(), 4U);
}

// Si lo que cambia una acción no lo explica el ruido, es un efecto: con p = 0.01, 2p(1 - p) =
// 0.0198, y (lit ?1) cambia en 1 de 4 (con alpha 0.3, el ruido explica 0). move corta (lit a) y
// cada tramo tiene una sola observación: no cambia nada.
TEST(SmoothAtoms, WhatTheNoiseDoesNotExplainIsAnEffect) {
  std::vector<plantraces::Trace> traces{shuttle()};
  plantraces::smooth_atoms(traces, 0.01);
  EXPECT_EQ(truth(traces.front(), 2, "(lit a)"), false);
  // (lit c) no es de ninguna acción: sigue sin una mayoría clara.
  EXPECT_EQ(truth(traces.front(), 0, "(lit c)"), std::nullopt);
}

// Las trazas de la variante de las trazas de prueba y, en el mismo orden, las completas.
std::pair<std::vector<plantraces::Trace>, std::vector<plantraces::Trace>> with_complete(
    const std::string& collection, const std::string& variant) {
  namespace fs = std::filesystem;
  std::vector<std::string> problems;
  for (const auto& entry : fs::directory_iterator(fs::path{PLANMINER_TRACES_DIR} / collection)) {
    const std::string name = entry.path().filename().string();
    if (name.ends_with(".complete.json")) {
      problems.push_back(name.substr(0, name.size() - std::string{".complete.json"}.size()));
    }
  }
  std::sort(problems.begin(), problems.end());
  std::pair<std::vector<plantraces::Trace>, std::vector<plantraces::Trace>> result;
  for (const std::string& problem : problems) {
    const fs::path directory = fs::path{PLANMINER_TRACES_DIR} / collection;
    result.first.push_back(plantraces::read_trace(directory / (problem + "." + variant + ".json")));
    result.second.push_back(plantraces::read_trace(directory / (problem + ".complete.json")));
  }
  return result;
}

// De las observaciones de los átomos con argumentos, cuántas están mal y cuántas son desconocidas.
std::pair<std::size_t, std::size_t> errors(const std::vector<plantraces::Trace>& traces,
                                           const std::vector<plantraces::Trace>& complete) {
  std::size_t wrong = 0;
  std::size_t unknown = 0;
  for (std::size_t t = 0; t < traces.size(); ++t) {
    for (std::size_t s = 0; s < traces[t].states.size(); ++s) {
      const plantraces::Observation& truth = complete[t].states[s];
      for (const auto* atoms : {&truth.true_atoms, &truth.false_atoms}) {
        for (const plantraces::GroundAtom& atom : *atoms) {
          if (atom.arguments.empty()) {
            continue;
          }
          const std::optional<bool> value = traces[t].states[s].truth(atom);
          if (!value) {
            ++unknown;
          } else if (*value != *truth.truth(atom)) {
            ++wrong;
          }
        }
      }
    }
  }
  return {wrong, unknown};
}

// En las trazas de prueba de blocksworld con un 5 % de ruido, de las 3956 observaciones de átomos
// con argumentos, el ruido ha invertido 192. Después de suavizar, con el p que se estima, quedan 6
// mal, y 55 pasan a desconocidas.
TEST(SmoothAtoms, TheNoiseOfBlocksworld) {
  auto [noisy, complete] = with_complete("blocks-strips-untyped", "noise5");
  EXPECT_EQ(errors(noisy, complete), (std::pair<std::size_t, std::size_t>{192, 0}));
  plantraces::smooth_atoms(noisy, plantraces::observation_noise(plantraces::frame_changes(noisy)));
  EXPECT_EQ(errors(noisy, complete), (std::pair<std::size_t, std::size_t>{6, 55}));
}

}  // namespace
