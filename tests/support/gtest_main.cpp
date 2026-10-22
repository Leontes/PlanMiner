// El main de los tests de GoogleTest de PlanMiner.
//
// Antes de nada fija un límite duro de memoria para el proceso (RLIMIT_DATA): un test que se
// dispara falla con std::bad_alloc en lugar de agotar la memoria de la máquina, que en WSL congela
// todo. El límite es PLANMINER_TEST_MEMORY_MB (por defecto, 2048 MB; 0, sin límite). Con ctest en
// paralelo, el peor caso es el número de trabajos por el límite.

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "resources/memory.hpp"

namespace {

constexpr double default_limit_mb = 2048;

}  // namespace

int main(int argc, char** argv) {
  double limit = default_limit_mb;
  if (const char* value = std::getenv("PLANMINER_TEST_MEMORY_MB")) {
    try {
      limit = std::stod(value);
    } catch (const std::exception&) {
      std::cerr << "PLANMINER_TEST_MEMORY_MB no es un número: " << value << '\n';
      return 2;
    }
  }
  if (limit > 0) {
    resources::limit_process(limit);
  }
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
