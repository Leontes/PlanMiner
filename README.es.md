# PlanMiner

[English](README.md) · **Español**

Aprendizaje de dominios de planificación PDDL a partir de trazas de planes, incluso si están
incompletas o tienen ruido. PlanMiner descubre las precondiciones y los efectos de cada acción,
también las expresiones aritméticas de los fluentes numéricos.

Es una reescritura en C++20 del PlanMiner original, con mejoras sobre el algoritmo publicado
([Segura-Muros, Pérez y Fernández-Olivares, 2021](https://doi.org/10.1007/s10489-021-02232-6)).
El algoritmo original se puede seguir usando (`--original`), y cada mejora es separable y
medible frente a él.

## Qué aprende

| Lenguaje | Qué incluye | Estado |
|----------|-------------|--------|
| STRIPS | precondiciones, añadidos y borrados, con tipos y costes de acción | sí |
| PDDL 2.1 sin tiempo | fluentes numéricos: precondiciones con comparaciones y efectos con expresiones aritméticas | sí |
| ADL | cuantificadores, efectos condicionales, disyunciones y constantes del dominio | no |
| PDDL 2.1 temporal | acciones durativas | no |

Con trazas completas, o con un 30 % de los átomos y valores desconocidos, la configuración
mejorada da planes válidos en todos los problemas de prueba de 14 de los 15 dominios del banco de
pruebas (el que falta usa constantes del dominio). Con un 5 % de ruido, todavía no:
ver [Evaluación](docs/es/evaluacion.md#resultados).

## Características

- **Entrada:** trazas en el formato de [PDDLDomains](https://github.com/Leontes/PDDLDomains)
  (`pddldomains trace`): los estados por los que pasa la ejecución de un plan, en JSON, con
  huecos, estados que faltan y ruido.
- **Precondiciones y efectos lógicos:** reglas que describen el estado de antes de cada acción y el
  de después, aprendidas con [INSLV](https://github.com/Leontes/INSLV-PlanMiner) (reglas difusas,
  con el genético de NSLV) y fusionadas.
- **Efectos numéricos:** regresión simbólica, una búsqueda A* de la expresión aritmética de cada
  cambio: `(decrease (fuel ?a) (* (distance ?c1 ?c2) (slow-burn ?a)))`.
- **Precondiciones numéricas:** comparaciones entre fluentes y con las expresiones de los
  efectos: `(>= (fuel ?a) (* (distance ?c1 ?c2) (slow-burn ?a)))`.
- **Tipos:** la jerarquía se infiere de los objetos de las trazas.
- **Dos configuraciones:** la mejorada (por defecto) y la original. Las mejoras están descritas
  en [Cómo aprende PlanMiner](docs/es/algoritmo.md).
- **Memoria:** PlanMiner se pone un límite de memoria y sus búsquedas paran al llegar a él, en
  lugar de agotar la de la máquina.
- **Evaluación:** precisión, recall y F de un dominio aprendido frente al de referencia, y un
  banco de pruebas reproducible con validación cruzada que planifica con los dominios
  aprendidos y valida los planes con VAL.

## Instalación

Requiere Linux, CMake 3.25 o posterior, git y un compilador de C++20 (probado con GCC 13 y
Clang 18).

```bash
git clone --recursive https://github.com/Leontes/PlanMiner.git
cd PlanMiner
cmake --preset release
cmake --build --preset release      # el ejecutable: build/release/bin/planminer
```

| Preset | Para qué |
|--------|----------|
| `release` | uso normal y experimentos |
| `debug` | desarrollo |
| `asan` | Debug con AddressSanitizer y UBSan |
| `ci-gcc`, `ci-clang` | Release con los avisos como errores, como la integración continua |

Cada preset compila en `build/<preset>`. [INSLV](https://github.com/Leontes/INSLV-PlanMiner) se
descarga al configurar (FetchContent), fijado en una versión (`v1.0.0`). El submódulo
`vendor/PDDLDomains` (el catálogo de dominios) solo hace falta para los tests y para la
evaluación.

## Idioma

La ayuda de la línea de comandos, los mensajes y los errores están en español. Los
identificadores (opciones, claves de los ficheros de configuración, nombres del PDDL) están en
inglés. La documentación está en español y en inglés.

## Uso

### Desde la terminal

```bash
planminer -o dominio.pddl trazas/*.json         # configuración mejorada
planminer --original -o dominio.pddl trazas/*.json
planminer -n blocks -m 2048 trazas/*.json        # a la salida estándar, con 2 GB como mucho
```

PlanMiner lee las trazas de un mismo dominio y escribe el dominio aprendido en el fichero (o en
la salida estándar, sin `-o`).

| Opción | Para qué |
|--------|----------|
| `-o`, `--output <fichero>` | el fichero del dominio |
| `-n`, `--name <nombre>` | el nombre del dominio; por defecto, el de las trazas (algunos planificadores exigen que coincida con el de los problemas) |
| `-m`, `--memory-limit <MB>` | el límite de memoria; por defecto, el 80 % de la disponible |
| `--original` | aprende con el algoritmo del PlanMiner original, sin mejoras |
| `-h`, `--help`, `--version` | la ayuda y la versión |

Códigos de salida:

- 0: dominio aprendido;
- 1: no se han podido leer las trazas o escribir el dominio;
- 2: argumentos no válidos.

Con `--original` y un dominio numérico, el dominio aprendido declara las funciones pero no el
requisito `:numeric-fluents`, como el PlanMiner original, y algunos planificadores lo rechazan.

### Como librería

Cada parte del algoritmo es una librería estática de CMake (ver [Estructura](#estructura)).
`pddl` aprende el dominio de principio a fin:

```cpp
#include "pddl/domain.hpp"
#include "plantraces/trace.hpp"

std::vector<plantraces::Trace> traces = {plantraces::read_trace("p01.json"),
                                         plantraces::read_trace("p02.json")};
pddl::LearnedDomain domain = pddl::learn(traces);  // o pddl::learn(traces, pddl::Options::original())
std::string text = pddl::write("blocks", domain);
```

- **Errores:** las trazas que no se pueden leer lanzan `plantraces::TraceError`.
- **Opciones:** `pddl::Options` reúne las de cada parte (`Options{}` es la configuración
  mejorada y `Options::original()`, la original); cada mejora es un campo que se puede
  desactivar por separado.
- **Reproducibilidad:** el aprendizaje no usa el `rand()` global. La discretización sigue la
  secuencia del generador de INSLV, que es la del `rand()` de glibc, en el mismo orden que el
  original: la configuración original da los mismos ejemplos y reglas.
- **Memoria:** `resources::limit_process` pone al proceso un límite duro, como hace la línea de
  comandos.

## Estructura

| Ruta | Qué hay |
|------|---------|
| `apps/planminer/` | la línea de comandos |
| `libs/plantraces/` | lectura de las trazas, aplicaciones de cada acción, átomos sobre sus parámetros y jerarquía de tipos |
| `libs/datasets/` | la tabla de ejemplos de cada acción, los efectos numéricos (con la regresión), las relaciones numéricas y la discretización |
| `libs/regression/` | la regresión simbólica: expresiones aritméticas y su búsqueda A* |
| `libs/rules/` | los ejemplos de INSLV, la interpretación de sus reglas y su fusión en precondición y efecto |
| `libs/pddl/` | el aprendizaje de principio a fin, la simplificación de las precondiciones y la escritura en PDDL |
| `libs/resources/` | los límites de memoria |
| `vendor/PDDLDomains/` | submódulo: el catálogo de dominios de los tests y del banco de pruebas |
| `scripts/` | la comparación de dominios, el banco de pruebas, las trazas de prueba y la preparación del entorno |
| `benchmarks/` | las definiciones del banco de pruebas y sus huellas |
| `tests/` | tests unitarios, del planificador, de los scripts y del oráculo; `data/traces/`: las trazas de prueba |
| `cmake/` | los módulos de CMake: avisos, sanitizers, dependencias (entre ellas, INSLV) y herramientas de los tests |
| `docs/es/`, `docs/en/` | documentación, en español y en inglés |

`build/` (compilaciones), `tools/` (VAL compilado en local), `.venv/` (el entorno de Python),
`Old/` (el código original, el oráculo de los tests) y `privado/` (documentos de uso privado) no
están en git.

## Documentación

- [Cómo aprende PlanMiner](docs/es/algoritmo.md): las partes del algoritmo y las mejoras de la
  configuración mejorada.
- [Evaluación](docs/es/evaluacion.md): la comparación de dominios, el banco de pruebas y sus
  resultados.
- [Tests](docs/es/pruebas.md): las etiquetas, las trazas de prueba, el oráculo y los límites de
  memoria.
- [Licencias](LICENCIAS.md): la de PlanMiner, la de INSLV y la de los dominios de prueba.

## Desarrollo

```bash
git clone --recursive https://github.com/Leontes/PlanMiner.git   # con el catálogo
git submodule update --init vendor/PDDLDomains                    # o, en un clon ya hecho
scripts/setup_python.sh     # .venv con PyPlanner y pddldomains
scripts/build_val.sh        # compila VAL en tools/bin/Validate (necesita git, cmake y g++)
cmake --workflow --preset debug                                   # configura, compila y prueba
git ls-files -- '*.cpp' '*.hpp' | xargs clang-format --dry-run --Werror
ruff check . && ruff format --check . && mypy --strict tests/scripts scripts
```

- **Tests:** por etiquetas de CTest (`ctest --preset debug -L unit`): `unit`, `planner` (con
  PyPlanner, VAL y PDDLDomains) y `oracle` (solo con `Old/`, en local). Ver
  [Tests](docs/es/pruebas.md).
- **INSLV en local:** para probar una copia local en lugar de la versión fijada en
  `cmake/Inslv.cmake`, `cmake --preset debug -DFETCHCONTENT_SOURCE_DIR_INSLV=<directorio>`.
- **Dependencias de los tests:** GoogleTest 1.14 o posterior (si no está instalado, CMake lo
  descarga); para los del planificador, Python 3.12 o posterior con
  [PyPlanner](https://github.com/Leontes/PyPlanner) y pddldomains (`scripts/setup_python.sh`) y
  [VAL](https://github.com/KCL-Planning/VAL) (`scripts/build_val.sh`). Tras instalarlos hay que
  volver a configurar.
- **Integración continua:** compila y prueba con GCC y Clang, con los sanitizers y con los tests
  del planificador, y comprueba el formato (clang-format 18) y los scripts (ruff y mypy).

## Cómo citar

Si usas PlanMiner en una publicación, cita el artículo:

> José Á. Segura-Muros, Raúl Pérez y Juan Fernández-Olivares. Discovering relational and
> numerical expressions from plan traces for learning action models. *Applied Intelligence*,
> 51(11):7973–7989, 2021. [doi:10.1007/s10489-021-02232-6](https://doi.org/10.1007/s10489-021-02232-6)

```bibtex
@article{seguramuros2021planminer,
  author  = {Segura-Muros, Jos{\'e} {\'A}. and P{\'e}rez, Ra{\'u}l and Fern{\'a}ndez-Olivares, Juan},
  title   = {Discovering relational and numerical expressions from plan traces for learning action models},
  journal = {Applied Intelligence},
  volume  = {51},
  number  = {11},
  pages   = {7973--7989},
  year    = {2021},
  doi     = {10.1007/s10489-021-02232-6}
}
```

## Licencia

El código y la documentación se distribuyen con la [licencia MIT](LICENSE) (© 2026 José Segura).
[INSLV](https://github.com/Leontes/INSLV-PlanMiner), que se descarga al configurar y parte de NSLV,
de Raúl Pérez, tiene la licencia GPL-3.0, y el ejecutable `planminer`, que lo enlaza, queda sujeto a
ella. Los dominios y problemas del catálogo PDDLDomains (submódulo) tienen sus propias condiciones.
Ver [LICENCIAS.md](LICENCIAS.md).
