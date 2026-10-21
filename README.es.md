# PlanMiner

[English](README.md) · **Español**

PlanMiner aprende dominios de planificación PDDL a partir de trazas de planes, incluso si
están incompletas, y descubre las expresiones lógicas y aritméticas de las precondiciones y
los efectos.

## Estado

El proyecto se está **reescribiendo desde cero en C++20**, portando el algoritmo poco a poco.
La implementación original está en [`Old/`](Old/) y sirve de oráculo en los tests: cada
parte portada se compara con la original.

Por ahora solo está la infraestructura de compilación y de tests. La línea de comandos
`planminer` solo informa de la versión.

## Requisitos

- Linux.
- CMake 3.25 o posterior.
- Un compilador de C++20. Se ha probado con GCC 13 y Clang 18.
- GoogleTest 1.14 o posterior. Si no está instalado (`libgtest-dev`), CMake lo descarga.

Para los tests del planificador, además:

- Python 3.12 o posterior, con
  [PyPlanner](https://github.com/Leontes/PyPlanner) (`scripts/setup_python.sh`).
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
| `release` | experimentos |
| `asan` | Debug con AddressSanitizer y UBSan; los tests solo pasan los unitarios |
| `ci-gcc`, `ci-clang` | la CI: Release con los avisos como errores |

Cada preset compila en `build/<preset>`.

### Tests

Los tests van por etiquetas de CTest (`ctest --preset debug -L <etiqueta>`):

| Etiqueta | Qué comprueba |
|----------|---------------|
| `unit` | el código nuevo |
| `oracle` | la comparación con `Old/`. Hay tests de las funciones originales, que se enlazan con el target `planminer::legacy`, y del dominio que aprende el original con las trazas de `Old/Test`, comparado con el guardado en `tests/oracle/golden/` |
| `planner` | los dominios aprendidos: PyPlanner planifica con ellos y VAL valida los planes con el dominio de referencia (`tests/planner/evaluate_domain.py`) |

Los tests del planificador solo se registran si están PyPlanner y VAL:

```bash
scripts/setup_python.sh   # .venv con PyPlanner
scripts/build_val.sh      # tools/bin/Validate
cmake --preset debug      # volver a configurar para que los detecte
```

Para regenerar los dominios de referencia del oráculo:

```bash
PLANMINER_UPDATE_GOLDEN=1 ctest --preset debug -R oracle.legacy
```

## Estructura

| Ruta | Qué hay |
|------|---------|
| `include/planminer/`, `src/` | la librería `planminer::core` |
| `apps/planminer/` | la línea de comandos |
| `tests/unit/` | tests unitarios |
| `tests/oracle/` | comparaciones con `Old/` y dominios de referencia (`golden/`) |
| `tests/planner/` | validación de dominios con PyPlanner y VAL |
| `tests/data/` | dominios y problemas de referencia |
| `cmake/` | módulos de CMake: avisos, sanitizers, dependencias, `Old/` y planificador |
| `scripts/` | preparación de PyPlanner y VAL |
| `Old/` | la implementación original, con un parche mínimo para que compile |

## Convenciones

- Los identificadores van en inglés; los comentarios, la documentación y los mensajes, en
  español.
- El formato de C++ es el de `.clang-format` (clang-format 18). Python se revisa con `ruff`
  y `mypy --strict`.
- Los avisos del compilador no se toleran en el código nuevo: la CI compila con `-Werror`.

## Licencia

[GPL-3.0](LICENSE).
