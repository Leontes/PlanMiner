#include <gtest/gtest.h>

#include <cstddef>
#include <cstdlib>
#include <new>
#include <vector>

#include "resources/memory.hpp"

namespace {

TEST(Memory, MeasuresTheProcessAndTheSystem) {
  ASSERT_TRUE(resources::resident_memory_mb());
  ASSERT_TRUE(resources::data_memory_mb());
  ASSERT_TRUE(resources::available_memory_mb());
  EXPECT_GT(*resources::resident_memory_mb(), 0.0);
  EXPECT_GT(*resources::data_memory_mb(), 0.0);
  EXPECT_GT(*resources::available_memory_mb(), 0.0);
}

TEST(Memory, TheDefaultLimitIsMostOfTheAvailableMemory) {
  const double available = *resources::available_memory_mb();
  const double limit = *resources::default_memory_limit_mb();
  EXPECT_LE(limit, available);
  EXPECT_GE(limit, 0.7 * available);  // el 80 %, medido un poco después
}

TEST(Memory, TheHardLimitHasAMargin) {
  // Al menos lo que ya ocupa el proceso, más el mayor de 512 MB y el 25 % del límite blando.
  EXPECT_GE(resources::hard_limit_mb(1000), 1000 + 512);
  EXPECT_GE(resources::hard_limit_mb(4000), 4000 + 1000);
}

// En un proceso hijo: un proceso no puede volver a subir su límite.
TEST(MemoryDeathTest, AboveTheHardLimitAllocationsFail) {
  if (!resources::hard_limit_supported()) {
    GTEST_SKIP() << "sin límite duro (por ejemplo, con AddressSanitizer)";
  }
  GTEST_FLAG_SET(death_test_style, "threadsafe");
  EXPECT_EXIT(
      {
        resources::limit_process(*resources::data_memory_mb() + 64);
        try {
          std::vector<char> big(std::size_t{512} << 20);
          big[0] = 1;
        } catch (const std::bad_alloc&) {
          std::exit(0);
        }
        std::exit(1);
      },
      testing::ExitedWithCode(0), "");
}

}  // namespace
