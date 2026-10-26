// La fusión del original. Que dé lo mismo que el original lo comprueba el oráculo
// (tests/oracle/test_legacy_rules.cpp); aquí, casos pequeños que muestran lo que hace, también
// sus fallos.

#include <gtest/gtest.h>

#include <ostream>
#include <string>
#include <vector>

#include "rules/fusion.hpp"

namespace rules {

void PrintTo(const Literal& literal, std::ostream* out) {
  *out << "[" << literal.attribute << " = " << literal.value << "]";
}

}  // namespace rules

namespace {

using rules::LegacyRule;
using rules::Literal;

// Los átomos pasan tal cual; el tipo de cada parámetro, el de la cabecera.
TEST(FuseOriginal, AtomsWithTheHeaderTypes) {
  const std::vector<LegacyRule> input = {
      {-4, {{"( on ?param_0 ~ object ?param_1 ~ object )", "True"}}},
      {4, {{"( on ?param_0 ~ object ?param_1 ~ object )", "False"}}},
  };
  const rules::Fused fused = rules::fuse_original(input, {"block", "block"});
  EXPECT_EQ(fused.precondition,
            (std::vector<Literal>{{"( on ?param_0 ~ ?block ?param_1 ~ ?block ) ", "True"}}));
  EXPECT_EQ(fused.effect,
            (std::vector<Literal>{{"( on ?param_0 ~ ?block ?param_1 ~ ?block ) ", "False"}}));
}

// Con dos valores para un atributo, el primero gana si cubre más del 5 % de los ejemplos de los
// dos: aquí, aunque cubra uno de cada once.
TEST(FuseOriginal, TheFirstValueAlmostAlwaysWins) {
  const std::vector<LegacyRule> input = {
      {-1, {{"( p )", "False"}}},
      {-10, {{"( p )", "True"}}},
  };
  EXPECT_EQ(rules::fuse_original(input, {}).precondition,
            (std::vector<Literal>{{"( p )", "False"}}));
}

// Una relación entre fluentes queda con una comparación; una con un número, como intervalo, con la
// frontera escrita con std::to_string; un valor discretizado se pierde.
TEST(FuseOriginal, Comparisons) {
  const std::vector<LegacyRule> input = {
      {-6, {{"opGT - ( x )  ( y ) ", "True"}, {"opLT - ( x )  1 ", "True"}, {"( x )", "5"}}},
  };
  // En el orden de los grupos del original: por el lado derecho, y "( y ) " va antes que los
  // números.
  EXPECT_EQ(
      rules::fuse_original(input, {}).precondition,
      (std::vector<Literal>{{"opGT - ( x ) ( y ) ", "True"}, {"opLT - ( x ) 1.000000", "True"}}));
}

// El cambio de un fluente con expresión va al efecto con su signo, y lo que ya está en la
// precondición no se repite.
TEST(FuseOriginal, Changes) {
  const std::vector<LegacyRule> input = {
      {-3, {{"( p )", "True"}}},
      {3, {{"DELTA - ( x ) ( * ( y ) ( z ) ) ", "-1"}, {"( p )", "True"}}},
  };
  const rules::Fused fused = rules::fuse_original(input, {});
  EXPECT_EQ(fused.effect, (std::vector<Literal>{{"DELTA - ( x ) ( * ( y ) ( z ) ) ", "-1"}}));
}

}  // namespace
