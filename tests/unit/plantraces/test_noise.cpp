#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <set>
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

// Lo que cambia fuera de la acción en los valores: (fuel p2) sí y (capacity p2) no; los átomos no
// cuentan.
TEST(ValueChanges, WhatChangesOutsideTheAction) {
  EXPECT_DOUBLE_EQ(plantraces::value_changes({flight(R"json(
      {"step": 1, "time": 1, "true": ["(located p1 c1)"], "unknown": [],
       "values": {"(fuel p1)": 6, "(fuel p2)": 4, "(capacity p2)": 9}, "unknown_values": []}
  )json")}),
                   0.5);
}

// c = 1 - (1 - p)^2: con un 10 % de ruido en cada valor, c = 0.19.
TEST(ValueNoise, TheInverseOfTheChanges) {
  EXPECT_EQ(plantraces::value_noise(0.0), 0.0);
  EXPECT_NEAR(plantraces::value_noise(1.0 - 0.9 * 0.9), 0.1, 1e-12);
  EXPECT_EQ(plantraces::value_noise(1.0), 1.0);
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

// Un robot r que va y viene entre a y b, con cuatro move. (size a) y (size b) no cambian, y en el
// estado 2 el ruido ha alterado (size a); (charge r) baja con cada move. (size c), de una
// habitación que no es de ninguna acción, sale 7 dos veces y 7.5 dos veces. (total-cost) y
// (max-charge) no tienen argumentos: el primero sube con cada move, y el segundo es una constante
// que el ruido ha alterado en el estado 3.
plantraces::Trace rover() {
  const auto state = [](int step, double size_a, double size_c, double charge) {
    return "{\"step\": " + std::to_string(step) + ", \"time\": " + std::to_string(step) +
           ", \"true\": [], \"unknown\": [], \"values\": {\"(size a)\": " + std::to_string(size_a) +
           ", \"(size b)\": 6, \"(size c)\": " + std::to_string(size_c) +
           ", \"(charge r)\": " + std::to_string(charge) +
           ", \"(total-cost)\": " + std::to_string(step) +
           ", \"(max-charge)\": " + (step == 3 ? "21" : "20") + "}, \"unknown_values\": []}";
  };
  return plantraces::parse_trace(
      R"json({"format": "pddldomains-trace", "version": 1, "domain": {"name": "rover"},
      "problem": {"name": "p"}, "world": "closed",
      "objects": {"r": "robot", "a": "room", "b": "room", "c": "room"}, "states": [)json" +
      state(0, 5, 7, 10) + "," + state(1, 5, 7.5, 9) + "," + state(2, 5.5, 7, 8) + "," +
      state(3, 5, 7.5, 7) + "," + state(4, 5, 8, 6) + R"json(], "actions": [
      {"step": 1, "time": 1, "name": "move", "arguments": ["r", "a", "b"]},
      {"step": 2, "time": 2, "name": "move", "arguments": ["r", "b", "a"]},
      {"step": 3, "time": 3, "name": "move", "arguments": ["r", "a", "b"]},
      {"step": 4, "time": 4, "name": "move", "arguments": ["r", "b", "a"]}]})json");
}

std::optional<double> value(const plantraces::Trace& trace, std::size_t state,
                            const std::string& term) {
  return trace.states[state].value(plantraces::parse_atom(term));
}

// Con p = 0.1, dos observaciones del mismo valor difieren con 1 - 0.9^2 = 0.19: (size ?1) y
// (size ?2) cambian en 1 de 4 move, que lo explica el ruido (con alpha 0.3, hasta 1); (charge ?0),
// en los 4: es un efecto.
TEST(SmoothValues, TheModeOfEachSegment) {
  std::vector<plantraces::Trace> traces{rover()};
  EXPECT_EQ(plantraces::smooth_values(traces, 0.1, false),
            (std::set<std::string>{"charge", "max-charge", "size"}));
  const plantraces::Trace& trace = traces.front();

  // (size a): 5 cuatro veces y 5.5 una, en un solo tramo: la moda.
  EXPECT_EQ(value(trace, 2, "(size a)"), 5.0);
  EXPECT_EQ(value(trace, 0, "(size b)"), 6.0);
  // (size c): dos 7 y dos 7.5, sin una moda clara: desconocido.
  for (std::size_t state = 0; state < 5; ++state) {
    EXPECT_EQ(value(trace, state, "(size c)"), std::nullopt) << state;
  }
  // (charge r): cada move lo corta, y una observación sola no se puede corroborar.
  EXPECT_EQ(value(trace, 1, "(charge r)"), std::nullopt);
  EXPECT_TRUE(trace.states[1].unknown_values.contains(plantraces::parse_atom("(charge r)")));
  // (total-cost) no se suaviza; (max-charge), la constante, sí, con la traza entera.
  EXPECT_EQ(value(trace, 3, "(total-cost)"), 3.0);
  EXPECT_EQ(value(trace, 3, "(max-charge)"), 20.0);
}

// De los valores de los fluentes con argumentos, cuántos están mal y cuántos son desconocidos.
std::pair<std::size_t, std::size_t> value_errors(const std::vector<plantraces::Trace>& traces,
                                                 const std::vector<plantraces::Trace>& complete) {
  std::size_t wrong = 0;
  std::size_t unknown = 0;
  for (std::size_t t = 0; t < traces.size(); ++t) {
    for (std::size_t s = 0; s < traces[t].states.size(); ++s) {
      for (const auto& [term, expected] : complete[t].states[s].values) {
        if (term.arguments.empty()) {
          continue;
        }
        const std::optional<double> observed = traces[t].states[s].value(term);
        if (!observed) {
          ++unknown;
        } else if (*observed != expected) {
          ++wrong;
        }
      }
    }
  }
  return {wrong, unknown};
}

// En las trazas de prueba de zenotravel con un 5 % de ruido, el ruido ha alterado 465 valores de
// fluentes con argumentos. Después de suavizar, con el p que se estima, no queda ninguno mal, y 716
// pasan a desconocidos: los que nada corrobora (problemas pequeños, con pocos estados entre las
// acciones que cambian cada fluente).
TEST(SmoothValues, TheNoiseOfZenotravel) {
  auto [noisy, complete] = with_complete("zenotravel", "noise5");
  EXPECT_EQ(value_errors(noisy, complete), (std::pair<std::size_t, std::size_t>{465, 0}));
  std::vector<plantraces::Trace> chained = noisy;
  const double p = plantraces::value_noise(plantraces::value_changes(noisy));
  plantraces::smooth_values(noisy, p, false);
  EXPECT_EQ(value_errors(noisy, complete), (std::pair<std::size_t, std::size_t>{0, 716}));
  // Con las cadenas, los valores entre efectos constantes (board, debark) se corroboran: 91
  // desconocidos.
  plantraces::smooth_values(chained, p);
  EXPECT_EQ(value_errors(chained, complete), (std::pair<std::size_t, std::size_t>{0, 91}));
}

// Un robot que se mueve 8 veces y gasta 1 de carga en cada movimiento: cada move cambia (charge
// r), y sin las cadenas cada observación está sola en su tramo. El cambio constante, -1, une los
// tramos: menos lo gastado, la carga es 10 en los 9 estados, salvo en el 4, que el ruido ha
// alterado (5.5 en lugar de 6).
TEST(SmoothValues, ChainsOfConstantChanges) {
  std::string states;
  std::string actions;
  for (int s = 0; s <= 8; ++s) {
    const std::string charge = s == 4 ? "5.5" : std::to_string(10 - s);
    states += std::string{s > 0 ? "," : ""} + R"j({"step": )j" + std::to_string(s) +
              R"j(, "time": 0, "true": [], "unknown": [], "values": {"(charge r)": )j" + charge +
              R"j(}, "unknown_values": []})j";
    if (s > 0) {
      actions += std::string{s > 1 ? "," : ""} + R"j({"step": )j" + std::to_string(s) +
                 R"j(, "time": 0, "name": "move", "arguments": ["r"]})j";
    }
  }
  const plantraces::Trace trace = plantraces::parse_trace(
      R"j({"format": "pddldomains-trace", "version": 1, "domain": {"name": "d"},
      "problem": {"name": "p"}, "world": "closed", "objects": {"r": "robot"}, "states": [)j" +
      states + R"j(], "actions": [)j" + actions + "]}");

  std::vector<plantraces::Trace> segments{trace};
  plantraces::smooth_values(segments, 0.1, false);
  EXPECT_EQ(value(segments.front(), 3, "(charge r)"), std::nullopt);

  std::vector<plantraces::Trace> chains{trace};
  plantraces::smooth_values(chains, 0.1);
  for (int s = 0; s <= 8; ++s) {
    EXPECT_EQ(value(chains.front(), static_cast<std::size_t>(s), "(charge r)"), 10.0 - s) << s;
  }
}

}  // namespace
