// Línea de comandos de PlanMiner: aprende un dominio PDDL de trazas de planes.
//
// Aprende con la configuración mejorada (pddl::Options{}) o, con --original, con la del PlanMiner
// original. Códigos de salida: 0 si todo va bien, 1 si no se pueden leer las trazas o escribir el
// dominio, 2 si los argumentos no son válidos.
//
// Memoria: antes de aprender se pone un límite duro (RLIMIT_DATA), por defecto del 80 % de la
// memoria disponible más un margen, como la línea de comandos de PyPlanner: en WSL, agotar la
// memoria congela todo (ver resources/memory.hpp).

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <new>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "pddl/domain.hpp"
#include "plantraces/trace.hpp"
#include "resources/memory.hpp"
#include "version.hpp"

namespace {

constexpr int exit_error = 1;
constexpr int exit_usage = 2;

constexpr std::string_view usage =
    "uso: planminer [opciones] <traza.json>...\n"
    "\n"
    "Aprende un dominio de planificación PDDL a partir de trazas de planes, en el formato de\n"
    "PDDLDomains (pddldomains trace).\n"
    "\n"
    "opciones:\n"
    "  -o, --output <fichero>   escribe el dominio en el fichero (por defecto, en la salida\n"
    "                           estándar)\n"
    "  -n, --name <nombre>      el nombre del dominio (por defecto, el de las trazas)\n"
    "  -m, --memory-limit <MB>  límite de memoria, en MB (por defecto, el 80 % de la disponible)\n"
    "  --original               aprende con el algoritmo del PlanMiner original, sin mejoras\n"
    "  -h, --help               muestra esta ayuda y termina\n"
    "  --version                muestra la versión y termina\n";

struct Arguments {
  std::vector<std::filesystem::path> traces;
  std::optional<std::filesystem::path> output;
  std::optional<std::string> name;
  std::optional<double> memory_mb;
  bool original = false;
  bool help = false;
  bool version = false;
};

class UsageError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

// El valor de una opción: "-o fichero", "--output fichero" o "--output=fichero".
std::string value(std::span<char*> args, std::size_t& i, std::string_view option) {
  const std::string_view arg{args[i]};
  if (const std::size_t equals = arg.find('='); equals != std::string_view::npos) {
    return std::string{arg.substr(equals + 1)};
  }
  if (i + 1 >= args.size()) {
    throw UsageError{"falta el valor de " + std::string{option}};
  }
  return args[++i];
}

Arguments parse(std::span<char*> args) {
  Arguments result;
  for (std::size_t i = 1; i < args.size(); ++i) {
    const std::string_view arg{args[i]};
    const std::string_view option = arg.substr(0, arg.find('='));
    if (arg == "-h" || arg == "--help") {
      result.help = true;
    } else if (arg == "--version") {
      result.version = true;
    } else if (arg == "--original") {
      result.original = true;
    } else if (option == "-o" || option == "--output") {
      result.output = value(args, i, option);
    } else if (option == "-n" || option == "--name") {
      result.name = value(args, i, option);
    } else if (option == "-m" || option == "--memory-limit") {
      const std::string text = value(args, i, option);
      std::size_t used = 0;
      double megabytes = 0.0;
      try {
        megabytes = std::stod(text, &used);
      } catch (const std::exception&) {
        used = 0;
      }
      if (used != text.size() || !(megabytes > 0.0)) {
        throw UsageError{"límite de memoria no válido: " + text};
      }
      result.memory_mb = megabytes;
    } else if (arg.starts_with("-") && arg != "-") {
      throw UsageError{"opción no reconocida: " + std::string{arg}};
    } else {
      result.traces.emplace_back(arg);
    }
  }
  if (!result.help && !result.version && result.traces.empty()) {
    throw UsageError{"faltan las trazas"};
  }
  return result;
}

// El nombre del dominio de las trazas: tiene que ser el mismo en todas.
std::string domain_name(const std::vector<plantraces::Trace>& traces) {
  for (const plantraces::Trace& trace : traces) {
    if (trace.domain != traces.front().domain) {
      throw std::runtime_error{"las trazas son de dominios distintos: " + traces.front().domain +
                               " y " + trace.domain};
    }
  }
  return traces.front().domain;
}

}  // namespace

int main(int argc, char* argv[]) {
  Arguments arguments;
  try {
    arguments = parse(std::span<char*>(argv, static_cast<std::size_t>(argc)));
  } catch (const UsageError& error) {
    std::cerr << "error: " << error.what() << "\n\n" << usage;
    return exit_usage;
  }
  if (arguments.help) {
    std::cout << usage;
    return EXIT_SUCCESS;
  }
  if (arguments.version) {
    std::cout << "planminer " << planminer::version << " (" << planminer::compiler << ", "
              << planminer::build_type << ")\n";
    return EXIT_SUCCESS;
  }

  const std::optional<double> limit =
      arguments.memory_mb ? arguments.memory_mb : resources::default_memory_limit_mb();
  if (limit) {
    resources::limit_process(resources::hard_limit_mb(*limit));
  }

  try {
    std::vector<plantraces::Trace> traces;
    for (const std::filesystem::path& file : arguments.traces) {
      traces.push_back(plantraces::read_trace(file));
    }
    const std::string name = arguments.name ? *arguments.name : domain_name(traces);

    // El original usa el rand() global desde el estado inicial del programa.
    std::srand(1);
    const pddl::Options options = arguments.original ? pddl::Options::original() : pddl::Options{};
    const std::string domain = pddl::write(name, pddl::learn(traces, options), options);

    if (!arguments.output) {
      std::cout << domain << "\n";
    } else {
      std::ofstream out(*arguments.output);
      out << domain << "\n";
      if (!out) {
        throw std::runtime_error{"no se puede escribir " + arguments.output->string()};
      }
    }
  } catch (const std::bad_alloc&) {
    std::cerr << "error: memoria agotada";
    if (limit) {
      std::cerr << " (límite de " << *limit << " MB)";
    }
    std::cerr << "\n";
    return exit_error;
  } catch (const std::exception& error) {
    std::cerr << "error: " << error.what() << "\n";
    return exit_error;
  }
  return EXIT_SUCCESS;
}
