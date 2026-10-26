// La jerarquía de tipos del original. Que coincida con el original lo comprueba el oráculo
// (tests/oracle/test_legacy_types.cpp); aquí, un caso pequeño.

#include <gtest/gtest.h>

#include <map>
#include <string>
#include <vector>

#include "plantraces/types.hpp"

namespace {

// move lleva un camión y después un avión a una ciudad: el parámetro 0 de move y el argumento 0
// de at tienen dos tipos, y los dos pasan a tener un padre nuevo.
plantraces::Trace trace() {
  plantraces::Trace trace;
  trace.objects = {{"t", {"truck"}}, {"p", {"plane"}}, {"c", {"city"}}, {"d", {"city"}}};
  trace.actions = {{1, 1, "move", {"t", "c"}}, {2, 2, "move", {"p", "d"}}};
  for (std::size_t step = 0; step < 3; ++step) {
    plantraces::Observation state;
    state.step = step;
    state.true_atoms = {{"ready", {}}, {"at", {"t", step == 0 ? "d" : "c"}}, {"at", {"p", "c"}}};
    state.values = {{{"fuel", {"p"}}, 10.0}};
    trace.states.push_back(state);
  }
  return trace;
}

TEST(TypeHierarchy, ConflictsGetACommonParent) {
  const plantraces::TypeHierarchy hierarchy = plantraces::type_hierarchy({trace()});
  EXPECT_EQ(
      hierarchy.types,
      (std::vector<plantraces::Type>{
          {"truck", "PARENTTYPE0"}, {"city", ""}, {"plane", "PARENTTYPE0"}, {"PARENTTYPE0", ""}}));
  EXPECT_EQ(hierarchy.actions,
            (std::map<std::string, std::vector<std::string>>{{"move", {"PARENTTYPE0", "city"}}}));
  // ready no tiene argumentos: no tiene cabecera.
  EXPECT_EQ(hierarchy.predicates, (std::map<std::string, std::vector<std::string>>{
                                      {"at", {"PARENTTYPE0", "city"}}, {"fuel", {"plane"}}}));
}

}  // namespace
