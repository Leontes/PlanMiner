// La escritura del dominio y el aprendizaje de la configuración original. Que coincidan con el
// original lo comprueba el oráculo (tests/oracle/test_legacy_domain.cpp); aquí, el formato con un
// dominio pequeño y el aprendizaje con blocksworld.

#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

#include "pddl/domain.hpp"

namespace {

using datasets::AttributeKind;

// Una acción move(?0 - truck) con su precondición y su efecto ya fusionados.
pddl::LearnedDomain domain() {
  pddl::LearnedDomain domain;
  domain.hierarchy.types = {{"truck", "vehicle"}, {"vehicle", ""}};
  domain.hierarchy.actions = {{"move", {"truck"}}};
  domain.hierarchy.predicates = {{"at", {"truck"}}, {"fuel", {"truck"}}};

  pddl::LearnedAction move;
  move.name = "move";
  move.argument_types = {"truck"};
  move.logic.attributes = {{rules::AttributeKind::atom,
                            {AttributeKind::atom, {"at", {0}}},
                            std::nullopt,
                            std::nullopt,
                            {}}};
  move.numeric.attributes = {{rules::AttributeKind::value,
                              {AttributeKind::fluent, {"fuel", {0}}},
                              std::nullopt,
                              std::nullopt,
                              {}}};
  move.fused.precondition = {{"( at ?param_0 ~ ?truck ) ", "True"},
                             {"( gone ?param_0 ~ ?truck ) ", "False"},
                             {"opGET - ( fuel ?param_0 ~ ?truck ) 1.000000", "True"},
                             {"opGT - TIMESTAMP 1.000000", "True"}};
  move.fused.effect = {{"( at ?param_0 ~ ?truck ) ", "False"},
                       {"DELTA - ( fuel ?param_0 ~ ?truck ) 1 ", "-1"},
                       {"opEQ ( fuel ?param_0 ~ ?truck ) 3", "3"}};
  domain.actions.push_back(move);
  return domain;
}

// Los átomos falsos de la precondición y lo del instante se pierden; la cabecera, los tipos y las
// secciones salen como en el original.
TEST(WriteOriginal, Format) {
  EXPECT_EQ(pddl::write("Trucks", domain(), pddl::Options::original()),
            "(define (domain trucks)\n"
            "\t(:requirements :strips :typing)\n"
            "\t(:types\n"
            "\t\tvehicle - object\n"
            "\t\ttruck - vehicle\n"
            "\t)\n"
            "\t(:predicates\n"
            "\t\t(at ?param_0 - truck)\n"
            "\t)\n"
            "\t(:functions\n"
            "\t\t(fuel ?param_0 - truck)\n"
            "\t)\n"
            "\t(:action move\n"
            "\t\t:parameters (?param_0 - truck)\n"
            "\t\t:precondition\n"
            "\t\t\t(and\n"
            "\t\t\t\t( at ?param_0 ) \n"
            "\t\t\t\t(>= ( fuel ?param_0 ) 1.000000 )\n"
            "\t\t\t)\n"
            "\t\t:effect\n"
            "\t\t\t(and\n"
            "\t\t\t\t( not ( at ?param_0 )  )\n"
            "\t\t\t\t( decrease ( fuel ?param_0 ) 1  )\n"
            "\t\t\t\t( assign ( fuel ?param_0 ) 3 )\n"
            "\t\t\t)\n"
            "\t)\n"
            ")");
}

// La mejorada declara :numeric-fluents, escribe las igualdades con = y los efectos de la regresión,
// con su forma.
TEST(Write, ImprovedNumericEffects) {
  pddl::LearnedDomain learned = domain();
  using regression::Expression;
  const plantraces::LiftedAtom fuel{"fuel", {0}};
  learned.actions[0].fused.effect = {{"( at ?param_0 ~ ?truck ) ", "False"}};
  learned.actions[0].effects = {
      {fuel,
       datasets::EffectKind::decrease,
       Expression::binary(regression::Operation::multiply, Expression::variable(1),
                          Expression::constant(2)),
       {fuel, {"speed", {0}}},
       0.0},
      {fuel,
       datasets::EffectKind::assign,
       Expression::variable(1),
       {fuel, {"capacity", {0}}},
       0.0}};

  learned.actions[0].fused.precondition = {
      {"opEQ - ( fuel ?param_0 ~ ?truck ) ( capacity ?param_0 ~ ?truck ) ", "True"}};

  const std::string text = pddl::write("trucks", learned);
  EXPECT_NE(text.find("(:requirements :strips :typing :numeric-fluents)"), std::string::npos)
      << text;
  // Las igualdades, con = (el original escribe ==, que no es PDDL).
  EXPECT_NE(text.find("\t\t\t\t(= ( fuel ?param_0 ) ( capacity ?param_0 ) )\n"), std::string::npos)
      << text;
  EXPECT_NE(pddl::write("trucks", learned, pddl::Options::original())
                .find("\t\t\t\t(== ( fuel ?param_0 ) ( capacity ?param_0 ) )\n"),
            std::string::npos);
  EXPECT_NE(text.find("\t\t\t\t( decrease ( fuel ?param_0 ) ( * ( speed ?param_0 ) 2 )  )\n"),
            std::string::npos)
      << text;
  EXPECT_NE(text.find("\t\t\t\t( assign ( fuel ?param_0 ) ( capacity ?param_0 ) )\n"),
            std::string::npos)
      << text;
}

std::vector<plantraces::Trace> traces(const std::string& collection,
                                      const std::string& variant = "complete") {
  namespace fs = std::filesystem;
  std::vector<fs::path> files;
  for (const auto& entry : fs::directory_iterator(fs::path{PLANMINER_TRACES_DIR} / collection)) {
    if (entry.path().filename().string().ends_with("." + variant + ".json")) {
      files.push_back(entry.path());
    }
  }
  std::sort(files.begin(), files.end());
  std::vector<plantraces::Trace> result;
  for (const fs::path& file : files) {
    result.push_back(plantraces::read_trace(file));
  }
  return result;
}

// Con la mejorada, los efectos numéricos de zenotravel salen de la regresión: los siete del
// dominio de referencia. En zenotravel, (distance ?c2 ?c1) vale lo mismo que (distance ?c1 ?c2):
// cualquiera de las dos es buena.
TEST(Learn, ImprovedZenotravelEffects) {
  const std::string text = pddl::write("zenotravel", pddl::learn(traces("zenotravel")));
  const std::string forward = "( distance ?param_1 ?param_2 )";
  const std::string backward = "( distance ?param_2 ?param_1 )";
  for (const std::string effect : {
           "( increase ( onboard ?param_1 ) 1  )",
           "( decrease ( onboard ?param_1 ) 1  )",
           "( decrease ( fuel ?param_0 ) ( * ( distance ?param_1 ?param_2 ) ( slow-burn ?param_0 ) "
           ")  )",
           "( increase ( total-fuel-used ) ( * ( distance ?param_1 ?param_2 ) ( slow-burn ?param_0 "
           ") )  )",
           "( decrease ( fuel ?param_0 ) ( * ( distance ?param_1 ?param_2 ) ( fast-burn ?param_0 ) "
           ")  )",
           "( increase ( total-fuel-used ) ( * ( distance ?param_1 ?param_2 ) ( fast-burn ?param_0 "
           ") )  )",
           "( assign ( fuel ?param_0 ) ( capacity ?param_0 ) )",
       }) {
    std::string swapped = effect;
    if (const std::size_t at = swapped.find(forward); at != std::string::npos) {
      swapped.replace(at, forward.size(), backward);
    }
    EXPECT_TRUE(text.find(effect) != std::string::npos || text.find(swapped) != std::string::npos)
        << effect << "\n"
        << text;
  }
}

// Mejora: sin ruido, la validación exige todos los ejemplos. En counters hay aplicaciones de
// decrement con valor 1: con min_support 0.95 pasaría (> (value ?0) 1); con todos, queda la de la
// referencia.
TEST(Learn, ExactWithoutNoise) {
  const std::string guard = "(>= ( value ?param_0 ) 1 )";
  EXPECT_NE(pddl::write("counters", pddl::learn(traces("counters"))).find(guard),
            std::string::npos);
  pddl::Options options;
  options.exact_without_noise = false;
  EXPECT_EQ(pddl::write("counters", pddl::learn(traces("counters"), options)).find(guard),
            std::string::npos);
}

// Las opciones, sin las cuatro de los numéricos con ruido.
pddl::Options without_numeric_noise(pddl::Options options = {}) {
  options.smooth_values = false;
  options.exact_comparisons = false;
  options.noisy_accumulators = false;
  options.noisy_null_model = false;
  return options;
}

// Las mejoras para el ruido (PlanMiner-N) solo cuentan si se estima ruido: con las trazas
// completas, la mejorada da lo mismo sin ellas, también con el umbral fijo de los valores raros.
TEST(Learn, NoiseImprovementsOnlyWithNoise) {
  pddl::Options fixed;
  fixed.rare_values_alpha = 0.0;
  pddl::Options without = without_numeric_noise(fixed);
  without.rare_values = 0.0;
  without.smooth_fluents = false;
  without.refine_meta_states = false;
  without.smooth_atoms = false;
  for (const char* collection : {"blocks-strips-untyped", "counters", "zenotravel"}) {
    const std::vector<plantraces::Trace> complete = traces(collection);
    const std::string expected = pddl::write(collection, pddl::learn(complete, without));
    EXPECT_EQ(pddl::write(collection, pddl::learn(complete)), expected) << collection;
    EXPECT_EQ(pddl::write(collection, pddl::learn(complete, fixed)), expected) << collection;
  }
}

// Con las cadenas de tramos y la regla de contraste con ruido, de las trazas de zenotravel con un
// 5 % de ruido sale la cota de fly-fast, (<= (onboard ?0) (zoom-limit ?0)), como de las
// completas; sin ellas, no.
TEST(Learn, ChainsAndNoisyContrast) {
  const std::string bound = "(<= ( onboard ?param_0 ) ( zoom-limit ?param_0 ) )";
  const std::vector<plantraces::Trace> noisy = traces("zenotravel", "noise5");
  pddl::Options without;
  without.value_chains = false;
  without.noisy_contrast = false;
  EXPECT_NE(pddl::write("zenotravel", pddl::learn(traces("zenotravel"))).find(bound),
            std::string::npos);
  EXPECT_NE(pddl::write("zenotravel", pddl::learn(noisy)).find(bound), std::string::npos);
  EXPECT_EQ(pddl::write("zenotravel", pddl::learn(noisy, without)).find(bound), std::string::npos);
}

// Los numéricos con ruido: con la mejorada, de las trazas de counters con un 5 % de ruido se
// aprende el mismo dominio que de las completas, con (>= (value ?0) 1) en decrement (sin las
// cuatro opciones, (> (value ?0) 1)). En zenotravel, ninguna precondición compara
// (total-fuel-used), que es un acumulador.
TEST(Learn, NumericNoise) {
  const std::string complete = pddl::write("counters", pddl::learn(traces("counters")));
  const std::vector<plantraces::Trace> noisy = traces("counters", "noise5");
  EXPECT_EQ(pddl::write("counters", pddl::learn(noisy)), complete);
  EXPECT_NE(pddl::write("counters", pddl::learn(noisy, without_numeric_noise())), complete);

  const std::string zenotravel =
      pddl::write("zenotravel", pddl::learn(traces("zenotravel", "noise5")));
  std::size_t comparisons = 0;
  std::size_t start = 0;
  for (std::size_t end = zenotravel.find('\n'); end != std::string::npos;
       start = end + 1, end = zenotravel.find('\n', start)) {
    const std::string line = zenotravel.substr(start, end - start);
    const bool comparison = line.find("\t\t\t\t(<") != std::string::npos ||
                            line.find("\t\t\t\t(>") != std::string::npos ||
                            line.find("\t\t\t\t(=") != std::string::npos;
    comparisons += comparison && line.find("total-fuel-used") != std::string::npos ? 1U : 0U;
  }
  EXPECT_EQ(comparisons, 0U) << zenotravel;
}

// Con ruido, el suavizado de los átomos cambia lo que se aprende. learn suaviza una copia de las
// trazas o, si se le dan para eso, las propias: el dominio es el mismo.
TEST(Learn, SmoothAtoms) {
  const std::vector<plantraces::Trace> noisy = traces("zenotravel", "noise5");
  const std::string copied = pddl::write("zenotravel", pddl::learn(noisy));
  EXPECT_EQ(pddl::write("zenotravel", pddl::learn(traces("zenotravel", "noise5"))), copied);
  pddl::Options without;
  without.smooth_atoms = false;
  EXPECT_NE(pddl::write("zenotravel", pddl::learn(noisy, without)), copied);
}

// Con ruido, los valores raros cambian lo que se aprende: con el umbral de la binomial (el de la
// mejorada) y con el fijo (lowFreq con 2c). En las trazas de prueba dan lo mismo; en el banco, no.
TEST(Learn, RareValuesWithNoise) {
  const std::vector<plantraces::Trace> noisy = traces("blocks-strips-untyped", "noise5");
  pddl::Options fixed;
  fixed.rare_values_alpha = 0.0;
  pddl::Options without = fixed;
  without.rare_values = 0.0;
  const std::string unfiltered = pddl::write("blocks", pddl::learn(noisy, without));
  EXPECT_NE(pddl::write("blocks", pddl::learn(noisy)), unfiltered);
  EXPECT_NE(pddl::write("blocks", pddl::learn(noisy, fixed)), unfiltered);
}

// Con las trazas completas de blocksworld, el original aprende el dominio de referencia. En la
// precondición fusionada también están los átomos falsos, que se pierden al escribirla.
TEST(LearnOriginal, Blocksworld) {
  const pddl::LearnedDomain learned =
      pddl::learn(traces("blocks-strips-untyped"), pddl::Options::original());
  ASSERT_EQ(learned.actions.size(), 4U);
  const std::string text = pddl::write("blocks", learned, pddl::Options::original());
  EXPECT_NE(text.find("\t(:action stack\n"
                      "\t\t:parameters (?param_0 - object ?param_1 - object)\n"
                      "\t\t:precondition\n"
                      "\t\t\t(and\n"
                      "\t\t\t\t( clear ?param_1 ) \n"
                      "\t\t\t\t( holding ?param_0 ) \n"
                      "\t\t\t)\n"),
            std::string::npos)
      << text;
}

}  // namespace
