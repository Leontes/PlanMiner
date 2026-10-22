// Línea de comandos de PlanMiner.
//
// Por ahora solo informa de la versión: los subcomandos llegarán a medida que se porte el
// algoritmo. Códigos de salida: 0 si todo va bien, 2 si los argumentos no son válidos.
//
// Memoria: antes de nada se pone un límite duro (RLIMIT_DATA) del 80 % de la memoria disponible
// más un margen, como la línea de comandos de PyPlanner: en WSL, agotar la memoria congela todo
// (ver resources/memory.hpp).

#include <cstdlib>
#include <iostream>
#include <optional>
#include <span>
#include <string_view>

#include "resources/memory.hpp"
#include "version.hpp"

namespace {

constexpr int exit_usage = 2;

constexpr std::string_view usage =
    "uso: planminer [--help] [--version]\n"
    "\n"
    "Aprende dominios de planificación PDDL a partir de trazas de planes.\n"
    "\n"
    "opciones:\n"
    "  -h, --help     muestra esta ayuda y termina\n"
    "  --version      muestra la versión y termina\n";

}  // namespace

int main(int argc, char* argv[]) {
  if (const std::optional<double> total = resources::default_memory_limit_mb()) {
    resources::limit_process(resources::hard_limit_mb(*total));
  }

  const std::span<char*> args(argv, static_cast<std::size_t>(argc));

  if (args.size() != 2) {
    std::cerr << usage;
    return exit_usage;
  }

  const std::string_view option{args[1]};
  if (option == "-h" || option == "--help") {
    std::cout << usage;
    return EXIT_SUCCESS;
  }
  if (option == "--version") {
    std::cout << "planminer " << planminer::version << " (" << planminer::compiler << ", "
              << planminer::build_type << ")\n";
    return EXIT_SUCCESS;
  }

  std::cerr << "error: opción no reconocida: " << option << "\n\n" << usage;
  return exit_usage;
}
