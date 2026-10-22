// Línea de comandos de PlanMiner.
//
// Por ahora solo informa de la versión: los subcomandos llegarán a medida que se porte el
// algoritmo. Códigos de salida: 0 si todo va bien, 2 si los argumentos no son válidos.

#include <cstdlib>
#include <iostream>
#include <span>
#include <string_view>

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
