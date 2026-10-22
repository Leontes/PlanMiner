// planminer_limit_memory <MB> <programa> [argumentos...]
//
// Ejecuta el programa con un límite duro de memoria (RLIMIT_DATA): si lo pasa, sus reservas de
// memoria fallan en lugar de agotar la de la máquina, que en WSL congela todo. Lo usan los tests
// que ejecutan programas que no son de PlanMiner, como el original de Old/.

#include <unistd.h>

#include <cstdio>
#include <exception>
#include <iostream>
#include <string>

#include "resources/memory.hpp"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "uso: planminer_limit_memory <MB> <programa> [argumentos...]\n";
    return 2;
  }
  double limit = 0.0;
  try {
    limit = std::stod(argv[1]);
  } catch (const std::exception&) {
    std::cerr << "error: el límite no es un número: " << argv[1] << '\n';
    return 2;
  }
  if (limit > 0 && !resources::limit_process(limit)) {
    std::cerr << "aviso: no se ha podido fijar el límite de memoria\n";
  }
  execvp(argv[2], argv + 2);
  std::perror(argv[2]);
  return 127;
}
