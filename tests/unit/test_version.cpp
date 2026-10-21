#include <gtest/gtest.h>

#include <string>

#include "planminer/version.hpp"

namespace {

TEST(Version, MatchesComponents) {
  const std::string expected = std::to_string(planminer::version_major) + "." +
                               std::to_string(planminer::version_minor) + "." +
                               std::to_string(planminer::version_patch);
  EXPECT_EQ(planminer::version, expected);
}

TEST(Version, BuildDescriptionStartsWithVersion) {
  const std::string prefix = "planminer " + std::string{planminer::version} + " (";
  EXPECT_TRUE(planminer::build_description().starts_with(prefix)) << planminer::build_description();
}

}  // namespace
