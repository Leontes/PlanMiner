// La escritura del dominio y el aprendizaje de la configuración original. Que coincidan con el
// original lo comprueba el oráculo (tests/oracle/test_legacy_domain.cpp); aquí, el formato con un
// dominio pequeño y el aprendizaje con blocksworld.

#include <gtest/gtest.h>

#include <algorithm>
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
// dominio de referencia (en zenotravel, (distance ?c2 ?c1) vale lo mismo que (distance ?c1 ?c2)).
TEST(Learn, ImprovedZenotravelEffects) {
  const std::string text = pddl::write("zenotravel", pddl::learn(traces("zenotravel")));
  for (const std::string effect : {
           "( increase ( onboard ?param_1 ) 1  )",
           "( decrease ( onboard ?param_1 ) 1  )",
           "( decrease ( fuel ?param_0 ) ( * ( distance ?param_1 ?param_2 ) ( slow-burn ?param_0 ) "
           ")  )",
           "( increase ( total-fuel-used ) ( * ( distance ?param_1 ?param_2 ) ( slow-burn ?param_0 "
           ") )  )",
           "( decrease ( fuel ?param_0 ) ( * ( distance ?param_2 ?param_1 ) ( fast-burn ?param_0 ) "
           ")  )",
           "( increase ( total-fuel-used ) ( * ( distance ?param_2 ?param_1 ) ( fast-burn ?param_0 "
           ") )  )",
           "( assign ( fuel ?param_0 ) ( capacity ?param_0 ) )",
       }) {
    EXPECT_NE(text.find(effect), std::string::npos) << effect << "\n" << text;
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

// Los valores raros (lowFreq) solo cuentan si se estima ruido: con las trazas completas no cambian
// nada, y con las de ruido, sí.
TEST(Learn, RareValuesOnlyWithNoise) {
  pddl::Options options;
  options.rare_values = 0.2;
  EXPECT_EQ(pddl::write("blocks", pddl::learn(traces("blocks-strips-untyped"), options)),
            pddl::write("blocks", pddl::learn(traces("blocks-strips-untyped"))));
  const std::vector<plantraces::Trace> noisy = traces("blocks-strips-untyped", "noise5");
  EXPECT_NE(pddl::write("blocks", pddl::learn(noisy, options)),
            pddl::write("blocks", pddl::learn(noisy)));
}

// El suavizado de los fluentes (HRC) solo cuenta si se estima ruido.
TEST(Learn, SmoothFluentsOnlyWithNoise) {
  pddl::Options options;
  options.smooth_fluents = true;
  EXPECT_EQ(pddl::write("zenotravel", pddl::learn(traces("zenotravel"), options)),
            pddl::write("zenotravel", pddl::learn(traces("zenotravel"))));
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
