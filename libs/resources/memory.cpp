#include "resources/memory.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

#ifdef __linux__
#include <sys/resource.h>
#include <unistd.h>
#endif

// AddressSanitizer: GCC define __SANITIZE_ADDRESS__ y Clang, __has_feature(address_sanitizer).
#if defined(__SANITIZE_ADDRESS__)
#define RESOURCES_ADDRESS_SANITIZER 1
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
#define RESOURCES_ADDRESS_SANITIZER 1
#endif
#endif

namespace resources {

namespace {

constexpr double kib_per_mib = 1024.0;
constexpr double bytes_per_mib = 1024.0 * 1024.0;

// El valor en kB de la línea "<clave>: <valor> kB" de un fichero de /proc, en MB.
std::optional<double> proc_field_mb(const char* file, const std::string& key) {
  std::ifstream in(file);
  for (std::string line; std::getline(in, line);) {
    if (line.starts_with(key)) {
      std::istringstream fields(line.substr(key.size()));
      double kib = 0.0;
      if (fields >> kib) {
        return kib / kib_per_mib;
      }
      return std::nullopt;
    }
  }
  return std::nullopt;
}

}  // namespace

std::optional<double> resident_memory_mb() {
#ifdef __linux__
  std::ifstream statm("/proc/self/statm");
  double size = 0.0;
  double pages = 0.0;
  if (statm >> size >> pages) {
    return pages * static_cast<double>(sysconf(_SC_PAGESIZE)) / bytes_per_mib;
  }
#endif
  return std::nullopt;
}

std::optional<double> data_memory_mb() { return proc_field_mb("/proc/self/status", "VmData:"); }

std::optional<double> available_memory_mb() {
  return proc_field_mb("/proc/meminfo", "MemAvailable:");
}

std::optional<double> default_memory_limit_mb() {
  if (const std::optional<double> available = available_memory_mb()) {
    return *available * default_memory_fraction;
  }
  return std::nullopt;
}

double hard_limit_mb(double soft) {
  const double current = data_memory_mb().value_or(0.0);
  return std::max(soft, current) + std::max(hard_margin_mb, soft * hard_margin_fraction);
}

bool hard_limit_supported() {
#if defined(__linux__) && !defined(RESOURCES_ADDRESS_SANITIZER)
  return true;
#else
  return false;
#endif
}

bool limit_process(double megabytes) {
#if defined(__linux__) && !defined(RESOURCES_ADDRESS_SANITIZER)
  rlimit limit{};
  if (getrlimit(RLIMIT_DATA, &limit) != 0) {
    return false;
  }
  auto bytes = static_cast<rlim_t>(megabytes * bytes_per_mib);
  if (limit.rlim_max != RLIM_INFINITY) {
    bytes = std::min(bytes, limit.rlim_max);
  }
  limit.rlim_cur = bytes;
  limit.rlim_max = bytes;
  return setrlimit(RLIMIT_DATA, &limit) == 0;
#else
  (void)megabytes;
  return false;
#endif
}

}  // namespace resources
