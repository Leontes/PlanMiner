# PlanMiner

[English](README.md) · **Español**

PlanMiner aprende dominios de planificación PDDL a partir de trazas de planes, incluso si
están incompletas, y descubre las expresiones lógicas y aritméticas de las precondiciones y
los efectos.

## Estado

El proyecto se está **reescribiendo desde cero en C++20**, portando el algoritmo poco a poco.
La implementación original está en [`Old/`](Old/) y sirve de oráculo en los tests: cada
parte portada se compara con la original.

Por ahora están la infraestructura de compilación y de tests y el lector de trazas. La línea
de comandos `planminer` solo informa de la versión.

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
- GoogleTest 1.14 o posterior. Si no está instalado (`libgtest-dev`), CMake lo descarga.

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
| `planner` | los dominios aprendidos: PyPlanner planifica con ellos y VAL valida los planes con el dominio de referencia (`tests/planner/evaluate_domain.py`). Y las trazas de prueba, que se tienen que regenerar igual |
| `inslv` | la librería INSLV de `vendor/inslv`, incluida su equivalencia con `Old/` |

Los tests del planificador solo se registran si están PyPlanner, pddldomains y VAL:

```bash
scripts/setup_python.sh   # .venv con PyPlanner y pddldomains
scripts/build_val.sh      # tools/bin/Validate
cmake --preset debug      # volver a configurar para que los detecte
```

Para regenerar las salidas de referencia del oráculo (los dominios de `tests/oracle/golden/` y
los casos de INSLV de `vendor/inslv/tests/cases/`):

```bash
PLANMINER_UPDATE_GOLDEN=1 ctest --preset debug -R "oracle.legacy|oracle.inslv_cases"
```

### Trazas de prueba

`tests/data/traces/` tiene trazas de problemas del catálogo PDDLDomains, con planes de
PyPlanner. Qué problemas y qué variantes (completa, mundo cerrado, con huecos, con ruido y con
estados que faltan) lo dice `tests/data/traces/dataset.toml`. Los planes se guardan para poder
regenerar las trazas sin volver a planificar:

```bash
.venv/bin/python scripts/generate_traces.py            # genera lo que falta y las trazas
.venv/bin/python scripts/generate_traces.py --replan   # también los planes
.venv/bin/python scripts/generate_traces.py --check    # comprueba que se regeneran igual
```

## Estructura

PlanMiner se organiza en librerías, una por cada parte del algoritmo, en `libs/<librería>`:
las fuentes en la raíz y las cabeceras públicas en `include/<librería>/`, que se incluyen como
`<librería>/<cabecera>.hpp`. Cada librería tiene su espacio de nombres y solo ve las
cabeceras de las librerías con las que enlaza. Sus tests unitarios están en
`tests/unit/<librería>`.

| Librería | Qué hace |
|----------|----------|
| `plantraces` | lee las trazas de plan y agrupa las aplicaciones de cada acción |
| `regression` | la regresión simbólica: el A* que busca la expresión de un efecto numérico |
| `datasets` | construye la tabla de ejemplos de cada acción y busca sus efectos numéricos |

| Ruta | Qué hay |
|------|---------|
| `libs/` | las librerías |
| `apps/planminer/` | la línea de comandos |
| `tests/unit/` | tests unitarios |
| `tests/oracle/` | comparaciones con `Old/` y dominios de referencia (`golden/`) |
| `tests/planner/` | validación de dominios con PyPlanner y VAL |
| `tests/data/` | dominios y problemas de referencia, y las trazas de prueba (`traces/`) |
| `cmake/` | módulos de CMake: librerías, avisos, sanitizers, dependencias, `Old/` y planificador |
| `scripts/` | preparación de PyPlanner, pddldomains y VAL, y generación de las trazas de prueba |
| `vendor/inslv/` | INSLV, el aprendizaje de reglas difusas (ver su [README](vendor/inslv/README.md)) |
| `vendor/PDDLDomains/` | submódulo: el catálogo de dominios y la utilidad de trazas |
| `Old/` | la implementación original, con un parche mínimo para que compile |

## INSLV

El aprendizaje de reglas difusas (INSLV) queda fuera del alcance de la reescritura. Está en
[`vendor/inslv`](vendor/inslv/) como dependencia externa, con una interfaz propia
(`inslv::learn`), hasta que tenga su propio repositorio.

## Convenciones

- Los identificadores van en inglés; los comentarios, la documentación y los mensajes, en
  español.
- El formato de C++ es el de `.clang-format` (clang-format 18). Python se revisa con `ruff`
  y `mypy --strict`.
- Los avisos del compilador no se toleran en el código nuevo: la CI compila con `-Werror`.

## Licencia

[GPL-3.0](LICENSE).
