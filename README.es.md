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
| `planner` | con PyPlanner, VAL y PDDLDomains: el arnés que valida dominios aprendidos, las trazas de prueba y el banco de pruebas mínimo |
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

### Comparar un dominio

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

### Banco de pruebas

`scripts/benchmark.py` evalúa PlanMiner con los dominios del catálogo de PDDLDomains que puede
aprender: STRIPS (con tipos y costes de acción) y PDDL 2.1 sin acciones durativas (fluentes
numéricos). Necesita el entorno de Python, VAL y PlanMiner compilado con el preset `release`:

```bash
.venv/bin/python scripts/benchmark.py generate   # problemas, planes y trazas
.venv/bin/python scripts/benchmark.py run        # validación cruzada
.venv/bin/python scripts/benchmark.py report     # tablas en build/benchmark/default/report.md
```

El banco se define en [`benchmarks/default.toml`](benchmarks/default.toml), que explica cada
ajuste:

- **Dominios.** Las colecciones que la selección pide a PDDLDomains. Se dejan fuera las que
  tienen un dominio por problema, porque PlanMiner aprende uno solo.
- **Problemas.** 90 por dominio, cada uno con su plan. En los clásicos son paseos aleatorios (el
  generador `random-walk` de PDDLDomains) desde los 6 problemas más pequeños de la colección, y
  el paseo es el plan. En los numéricos, el generador de la colección da los problemas y
  PyPlanner los planes; se descartan los problemas que no resuelve.
- **Cobertura.** De los paseos se generan 3 candidatos por problema y se eligen los que cubren
  el dominio: cada acción aplicada y cada efecto cambiando el estado en al menos 6 problemas, o
  en todos los candidatos que lo muestran, si son menos. Lo que no muestra ningún problema (una
  acción que no se puede aplicar en ningún problema de la colección) no cuenta en las métricas,
  porque no se puede aprender; el informe lo indica, junto con lo que falte en el entrenamiento
  de alguna partición.
- **Trazas.** Una por plan y variante: completa, con el 30 % de los átomos y valores
  desconocidos (`missing30`) y con un 5 % de ruido (`noise5`).
- **Validación cruzada.** Los problemas de cada dominio se reparten en 3 particiones. Con cada
  partición, variante y configuración (`--original` o no), PlanMiner aprende el dominio con las
  trazas de las demás particiones. El dominio aprendido se compara con el de referencia
  (precisión, recall y F en modo tolerante, y también exacto). Después, PyPlanner planifica con
  él los problemas de la partición, y VAL valida los planes con el dominio de referencia. La
  validez solo cuenta los problemas que resuelve el dominio de referencia.
- **Informe.** Para cada dominio, variante y configuración: la media y la desviación típica de
  las particiones en precisión, recall y F, y los planes válidos. Los planes que no son válidos
  se desglosan por causa: inválidos, sin solución, límite del planificador, error (PyPlanner
  rechaza el dominio) y sin dominio (PlanMiner ha fallado).

Todo es reproducible:

- Las semillas son fijas.
- `generate` comprueba las huellas de lo que genera con las de
  `benchmarks/default.lock.json`, y termina con 1 si no coinciden (`--update-lock` guarda las
  nuevas).
- `run` guarda el resultado de cada aprendizaje y sigue donde se quedó (`--force` los repite).

Otras opciones:

| Opción | Para qué |
|--------|----------|
| `--jobs N` | trabajos en paralelo (3 por defecto); tienen que caber en el 80 % de la memoria disponible |
| `--collections PATRÓN...` | solo estas colecciones (`'ipc-2011/*'`) |
| `--configurations NOMBRE...` | con `run`, solo estas configuraciones de PlanMiner (`original`, `improved`) |
| `--output DIR` | el directorio de salida (por defecto, `build/benchmark/<banco>`) |
| `--planminer FICHERO`, `--validate FICHERO` | los ejecutables de PlanMiner y de VAL |

Otro banco es otro TOML: `scripts/benchmark.py run mio.toml`.
[`benchmarks/smoke.toml`](benchmarks/smoke.toml) es el mínimo de los tests `planner`.

## Memoria

PlanMiner limita la memoria que usa, para no agotar la de la máquina (en WSL, agotarla
congela WSL entero):

- La línea de comandos se pone al empezar un límite del 80 % de la memoria disponible, o el de
  `-m`. Al llegar a él, las búsquedas paran y lo indican en lugar de seguir reservando memoria.
- Cada ejecutable de tests se limita a 2048 MB (`PLANMINER_TEST_MEMORY_MB`; 0, sin límite).
  Con `ctest` en paralelo (4 trabajos), el peor caso es 8 GB.
- Los tests lanzan PyPlanner siempre con límite de memoria (`-m`).
- El banco de pruebas lanza PlanMiner y PyPlanner con los límites de su definición (2560 MB y
  1024 MB por defecto), y no empieza si sus trabajos no caben en el 80 % de la memoria
  disponible.

## Licencia

[GPL-3.0](LICENSE).
