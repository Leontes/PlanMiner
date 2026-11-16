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

// Corrección: una condición de una regla se queda solo si se cumple en al menos min_support de
// los ejemplos de su estado. (p) es cierto en tres de los cuatro estados de antes.
TEST(Fuse, ValidatesTheConditionsWithTheExamples) {
  const std::vector<LegacyRule> input = {{-1, {{"( p )", "True"}, {"( q )", "True"}}}};
  rules::Evidence evidence;
  evidence.transitions = 4;
  evidence.columns = {{"( p )", {1, 1, 1, 0, 0, 0, 0, 0}}, {"( q )", {1, 1, 1, 1, 0, 0, 0, 0}}};

  rules::FusionOptions options = rules::FusionOptions::original();
  options.validate = true;
  EXPECT_EQ(rules::fuse(input, {}, evidence, options).precondition,
            (std::vector<Literal>{{"( q )", "True"}}));
  options.min_support = 0.7;
  EXPECT_EQ(rules::fuse(input, {}, evidence, options).precondition,
            (std::vector<Literal>{{"( p )", "True"}, {"( q )", "True"}}));
}

// Las comparaciones numéricas, con su propio min_support: con 1, (x > 1), cierta en tres de los
// cuatro estados de antes, se quita; (p), con el mismo soporte, se queda con el min_support de 0.7.
TEST(Fuse, TheSupportOfTheComparisons) {
  const std::vector<LegacyRule> input = {{-1, {{"( p )", "True"}, {"opGT - ( x )  1 ", "True"}}}};
  rules::Evidence evidence;
  evidence.transitions = 4;
  evidence.columns = {{"( p )", {1, 1, 1, 0, 0, 0, 0, 0}},
                      {"opGT - ( x )  1 ", {1, 1, 1, 0, 0, 0, 0, 0}}};

  rules::FusionOptions options = rules::FusionOptions::original();
  options.validate = true;
  options.min_support = 0.7;
  options.min_support = 0.7;
  EXPECT_EQ(rules::fuse(input, {}, evidence, options).precondition,
            (std::vector<Literal>{{"( p )", "True"}, {"opGT - ( x ) 1.000000", "True"}}));
  options.min_comparison_support = 1.0;
  EXPECT_EQ(rules::fuse(input, {}, evidence, options).precondition,
            (std::vector<Literal>{{"( p )", "True"}}));
}

// Corrección: en un conflicto gana el valor que cubre más ejemplos (con el original, el primero).
TEST(Fuse, TheMajorityWins) {
  const std::vector<LegacyRule> input = {
      {-1, {{"( p )", "False"}}},
      {-10, {{"( p )", "True"}}},
      {1, {{"( q )", "False"}}},
      {10, {{"( q )", "True"}}},
  };
  rules::FusionOptions options = rules::FusionOptions::original();
  options.majority = true;
  const rules::Fused fused = rules::fuse(input, {}, {}, options);
  EXPECT_EQ(fused.precondition, (std::vector<Literal>{{"( p )", "True"}}));
  EXPECT_EQ(fused.effect, (std::vector<Literal>{{"( q )", "True"}}));
}

// PlanMiner-N: fuera la condición de poco soporte, (s), y la del conflicto igualado, (t) con 50 y
// 48 ejemplos; (q), con 90 frente a 8, se queda.
TEST(Fuse, RefinesTheMetaStates) {
  const std::vector<LegacyRule> input = {
      {-50, {{"( p )", "True"}, {"( q )", "True"}, {"( t )", "True"}}},
      {-40, {{"( p )", "True"}, {"( q )", "True"}, {"( t )", "False"}}},
      {-8, {{"( p )", "True"}, {"( q )", "False"}, {"( t )", "False"}}},
      {-2, {{"( s )", "True"}}},
  };
  rules::Evidence evidence;
  evidence.transitions = 100;
  rules::FusionOptions options = rules::FusionOptions::original();
  options.majority = true;
  EXPECT_EQ(rules::fuse(input, {}, evidence, options).precondition,
            (std::vector<Literal>{
                {"( p )", "True"}, {"( q )", "True"}, {"( t )", "True"}, {"( s )", "True"}}));
  options.min_rule_support = 0.05;
  options.conflict_band = 0.1;
  EXPECT_EQ(rules::fuse(input, {}, evidence, options).precondition,
            (std::vector<Literal>{{"( p )", "True"}, {"( q )", "True"}}));
}

// Corrección: las comparaciones ciertas y falsas de una pareja se combinan. (x) no es menor que 1:
// >=. (x) no es menor ni igual que (y): >. (x) es igual y mayor que (z): contradicción, nada.
TEST(Fuse, CombinesTheComparisons) {
  const std::vector<LegacyRule> input = {
      {-5,
       {{"opLT - ( x )  1 ", "False"},
        {"opLT - ( x )  ( y ) ", "False"},
        {"opEQ - ( x )  ( y ) ", "False"},
        {"opEQ - ( x )  ( z ) ", "True"},
        {"opGT - ( x )  ( z ) ", "True"}}},
  };
  rules::FusionOptions options = rules::FusionOptions::original();
  options.combine_comparisons = true;
  EXPECT_EQ(rules::fuse(input, {}, {}, options).precondition,
            (std::vector<Literal>{{"opGT - ( x ) ( y ) ", "True"}, {"opGET - ( x ) 1", "True"}}));
  // El original no usa las falsas, ni junta = con >.
  EXPECT_EQ(rules::fuse_original(input, {}).precondition,
            (std::vector<Literal>{{"opGT - ( x ) ( z ) ", "True"}}));
}

}  // namespace
