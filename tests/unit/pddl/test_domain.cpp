// La escritura del dominio y el aprendizaje de la configuración original. Que coincidan con el
// original lo comprueba el oráculo (tests/oracle/test_legacy_domain.cpp); aquí, el formato con un
// dominio pequeño y el aprendizaje con blocksworld.

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
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
  EXPECT_EQ(pddl::write_original("Trucks", domain()),
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

std::vector<plantraces::Trace> blocksworld() {
  namespace fs = std::filesystem;
  std::vector<fs::path> files;
  for (const auto& entry :
       fs::directory_iterator(fs::path{PLANMINER_TRACES_DIR} / "blocks-strips-untyped")) {
    if (entry.path().filename().string().ends_with(".complete.json")) {
      files.push_back(entry.path());
    }
  }
  std::sort(files.begin(), files.end());
  std::vector<plantraces::Trace> traces;
  for (const fs::path& file : files) {
    traces.push_back(plantraces::read_trace(file));
  }
  return traces;
}

// Con las trazas completas de blocksworld, el original aprende el dominio de referencia. En la
// precondición fusionada también están los átomos falsos, que se pierden al escribirla.
TEST(LearnOriginal, Blocksworld) {
  std::srand(1);
  const pddl::LearnedDomain learned = pddl::learn_original(blocksworld());
  ASSERT_EQ(learned.actions.size(), 4U);
  const std::string text = pddl::write_original("blocks", learned);
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
