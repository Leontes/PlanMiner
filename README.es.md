# PlanMiner

[English](README.md) · **Español**

PlanMiner aprende dominios de planificación PDDL a partir de trazas de planes, incluso si
están incompletas, y descubre las expresiones lógicas y aritméticas de las precondiciones y
los efectos.

> **En desarrollo.** PlanMiner se está reescribiendo en C++20. La línea de comandos todavía
> no aprende dominios: por ahora solo informa de la versión.

## Trazas

PlanMiner aprende de trazas en el formato de
[PDDLDomains](https://github.com/Leontes/PDDLDomains) (`pddldomains trace`): los estados por
los que pasa la ejecución de un plan, con huecos, estados que faltan y ruido, y las acciones.
El formato está descrito en
[`vendor/PDDLDomains/docs/es/trazas.md`](https://github.com/Leontes/PDDLDomains/blob/main/docs/es/trazas.md).

## Requisitos

- Linux.
- CMake 3.25 o posterior.
- Un compilador de C++20. Se ha probado con GCC 13 y Clang 18.
- GoogleTest 1.14 o posterior, para los tests. Si no está instalado (`libgtest-dev`), CMake lo
  descarga.

Para los tests del planificador, además:

- El submódulo `vendor/PDDLDomains` (`git clone --recursive`, o
  `git submodule update --init` en un clon ya hecho).
- Python 3.12 o posterior, con [PyPlanner](https://github.com/Leontes/PyPlanner) y
  `pddldomains` (`scripts/setup_python.sh`).
- [VAL](https://github.com/KCL-Planning/VAL) (`scripts/build_val.sh`, necesita git, cmake y
  g++).

## Compilar y probar

```bash
cmake --workflow --preset debug     # configura, compila y pasa los tests

# o paso a paso
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

| Preset | Para qué |
|--------|----------|
| `debug` | desarrollo |
| `release` | uso normal y experimentos |
| `asan` | Debug con AddressSanitizer y UBSan; los tests solo pasan los unitarios |
| `ci-gcc`, `ci-clang` | Release con los avisos como errores |

Cada preset compila en `build/<preset>`. El ejecutable es `build/<preset>/bin/planminer`:

```bash
build/release/bin/planminer --version
```

Los tests van por etiquetas de CTest (`ctest --preset debug -L <etiqueta>`):

| Etiqueta | Qué comprueba |
|----------|---------------|
| `unit` | las librerías y la línea de comandos |
| `planner` | los dominios aprendidos: PyPlanner planifica con ellos y VAL valida los planes con el dominio de referencia |
| `inslv` | la librería de aprendizaje de reglas difusas (`vendor/inslv`) |

Los tests del planificador solo se registran si están PyPlanner, pddldomains y VAL:

```bash
scripts/setup_python.sh   # .venv con PyPlanner y pddldomains
scripts/build_val.sh      # tools/bin/Validate
cmake --preset debug      # volver a configurar para que los detecte
```

## Memoria

PlanMiner limita la memoria que usa, para no agotar la de la máquina (en WSL, agotarla
congela WSL entero):

- La línea de comandos se pone al empezar un límite del 80 % de la memoria disponible. Al
  llegar a él, las búsquedas paran y lo indican en lugar de seguir reservando memoria.
- Cada ejecutable de tests se limita a 2048 MB (`PLANMINER_TEST_MEMORY_MB`; 0, sin límite).
  Con `ctest` en paralelo (4 trabajos), el peor caso es 8 GB.
- Los tests lanzan PyPlanner siempre con límite de memoria (`-m`).

## Licencia

[GPL-3.0](LICENSE).
