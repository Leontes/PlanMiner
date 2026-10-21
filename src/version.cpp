#include "planminer/version.hpp"

namespace planminer {

std::string build_description() {
  std::string description{"planminer "};
  description += version;
  description += " (" PLANMINER_COMPILER ", " PLANMINER_BUILD_TYPE ")";
  return description;
}

}  // namespace planminer
