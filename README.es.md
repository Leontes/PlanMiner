# PlanMiner

[English](README.md) · **Español**

PlanMiner aprende dominios de planificación PDDL a partir de trazas de planes, incluso si
están incompletas, y descubre las expresiones lógicas y aritméticas de las precondiciones y
los efectos.

> **En desarrollo.** PlanMiner se está reescribiendo en C++20, con mejoras sobre el algoritmo
> original que se van incorporando poco a poco. El algoritmo original se puede seguir usando
> (`--original`).

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

Cada preset compila en `build/<preset>`, y el ejecutable queda en `build/<preset>/bin/planminer`.

Los tests van por etiquetas de CTest (`ctest --preset debug -L <etiqueta>`):

| Etiqueta | Qué comprueba |
|----------|---------------|
| `unit` | las librerías, la línea de comandos y los scripts |
| `planner` | los dominios aprendidos: PyPlanner planifica con ellos y VAL valida los planes con el dominio de referencia |
| `inslv` | la librería de aprendizaje de reglas difusas (`vendor/inslv`) |

Los tests del planificador solo se registran si están PyPlanner, pddldomains y VAL:

```bash
scripts/setup_python.sh   # .venv con PyPlanner y pddldomains
scripts/build_val.sh      # tools/bin/Validate
cmake --preset debug      # volver a configurar para que los detecte
```

## Uso

```bash
planminer -o dominio.pddl trazas/*.json
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

Termina con 0 si todo va bien, con 1 si no puede leer las trazas o escribir el dominio, y con 2
si los argumentos no son válidos.

Con `--original` y un dominio numérico, el dominio aprendido declara las funciones pero no el
requisito `:numeric-fluents`, como el PlanMiner original, y algunos planificadores lo rechazan.

## Evaluación

`scripts/compare_domains.py` compara un dominio aprendido con el de referencia (solo necesita
Python 3.12 o posterior):

```bash
scripts/compare_domains.py dominio.pddl referencia.pddl [--tolerant [--symmetric NOMBRE]...] \
    [--details] [--json]
```

Cada elemento de las precondiciones y los efectos de las acciones (un átomo, su negación, una
comparación numérica o un efecto numérico) cuenta como:

- verdadero positivo si está en los dos dominios;
- falso positivo si solo está en el aprendido (sobra);
- falso negativo si solo está en el de referencia (falta).

Sumando todas las acciones, da la precisión, el recall y el F-score de las precondiciones, de
los efectos y de todo junto. `--details` lista los elementos que sobran, los que faltan y los
medios aciertos de cada acción, y `--json` escribe el resultado en JSON.

Las acciones se emparejan por su nombre, y sus parámetros por su posición; los tipos no se
comparan. Antes de comparar, los elementos se normalizan (mayúsculas, formato de los números,
`>` como `<` con los lados cambiados, el orden de los operandos de `=`, `+` y `*`), pero no se
reconocen otras equivalencias: `(< a b)` y `(<= a b)` son elementos distintos.

Con `--tolerant`, un elemento que sobra y otro que falta casi iguales cuentan como medio acierto
(medio verdadero positivo, medio falso positivo y medio falso negativo) en lugar de como un
falso positivo y un falso negativo. Son casi iguales si solo se distinguen en `<` frente a `<=`,
o en el orden de los argumentos de un símbolo simétrico: una función o un predicado de dos
argumentos cuyo valor no depende de su orden, como la distancia entre dos ciudades, que se
indica con `--symmetric` (una vez por símbolo).

Termina con 0 si ha podido comparar los dominios y con 2 si los argumentos o los dominios no son
válidos.

## Memoria

PlanMiner limita la memoria que usa, para no agotar la de la máquina (en WSL, agotarla
congela WSL entero):

- La línea de comandos se pone al empezar un límite del 80 % de la memoria disponible, o el de
  `-m`. Al llegar a él, las búsquedas paran y lo indican en lugar de seguir reservando memoria.
- Cada ejecutable de tests se limita a 2048 MB (`PLANMINER_TEST_MEMORY_MB`; 0, sin límite).
  Con `ctest` en paralelo (4 trabajos), el peor caso es 8 GB.
- Los tests lanzan PyPlanner siempre con límite de memoria (`-m`).

## Licencia

[GPL-3.0](LICENSE).
