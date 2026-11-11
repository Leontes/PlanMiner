# Evaluación

[English](../en/evaluation.md) · **Español**

Dos herramientas: `scripts/compare_domains.py` compara un dominio aprendido con el de
referencia, y `scripts/benchmark.py` evalúa PlanMiner con un banco de pruebas reproducible.

## Comparar un dominio

```bash
scripts/compare_domains.py dominio.pddl referencia.pddl [--tolerant [--symmetric NOMBRE]...] \
    [--details] [--json]
```

Solo necesita Python 3.12 o posterior. Cada elemento de las precondiciones y los efectos de las
acciones (un átomo, su negación, una comparación numérica o un efecto numérico) cuenta como:

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

## El banco de pruebas

`scripts/benchmark.py` evalúa PlanMiner con los dominios del catálogo de PDDLDomains que puede
aprender: STRIPS (con tipos y costes de acción) y PDDL 2.1 sin acciones durativas (fluentes
numéricos). Necesita el entorno de Python, VAL y PlanMiner compilado con el preset `release`:

```bash
.venv/bin/python scripts/benchmark.py generate   # problemas, planes y trazas
.venv/bin/python scripts/benchmark.py run        # validación cruzada
.venv/bin/python scripts/benchmark.py report     # tablas en build/benchmark/default/report.md
```

El banco se define en [`benchmarks/default.toml`](../../benchmarks/default.toml), que explica
cada ajuste:

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
- **Trazas.** Una por plan y variante: completa (`complete`), con el 30 % de los átomos y valores
  desconocidos (`missing30`) y con un 5 % de ruido (`noise5`).
- **Validación cruzada.** Los problemas de cada dominio se reparten en 3 particiones. Con cada
  partición, variante y configuración (`--original` o no), PlanMiner aprende el dominio con las
  trazas de las demás particiones. El dominio aprendido se compara con el de referencia
  (precisión, recall y F en modo tolerante, y también exacto). Después, PyPlanner planifica con
  él los problemas de la partición, y VAL valida los planes con el dominio de referencia. La
  validez solo cuenta los problemas que resuelve el dominio de referencia.
- **Informe.** Para cada dominio, variante y configuración: la media y la desviación típica de
  las particiones en precisión, recall y F, y los planes válidos. Los planes que no son válidos
  se desglosan por causa: inválidos (`i`, VAL los rechaza), sin solución (`u`), límite del
  planificador (`l`), error (`e`, PyPlanner rechaza el dominio) y sin dominio (`f`, PlanMiner ha
  fallado).

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
[`benchmarks/smoke.toml`](../../benchmarks/smoke.toml) es el mínimo de los tests `planner`, y
[`benchmarks/ruido.toml`](../../benchmarks/ruido.toml), el de ruido: los mismos dominios, problemas
y planes que `default`, con trazas completas y con un 3, 5, 10 y 20 % de ruido, sin huecos (las
completas y las del 5 % son las de `default`; `scripts/benchmark.py generate ruido`, `run ruido`).

El banco lanza PlanMiner y PyPlanner con los límites de memoria de su definición (2560 MB y
1024 MB por defecto), y no empieza si sus trabajos no caben en el 80 % de la memoria disponible.
El límite de tiempo de PyPlanner (`-t`) solo cubre la búsqueda, así que su ejecución entera se
corta al doble.

## Resultados

El banco `default` con PlanMiner en el commit `f69da15` (octubre de 2026): 15 dominios, 90
problemas por dominio y 3 particiones. P, R y F en modo tolerante; la validez es el porcentaje de
planes válidos. INSLV 1.0 da los mismos resultados: con él, los 270 dominios aprendidos son
idénticos byte a byte.

| Clase | Variante | Original: F · validez | Mejorada: F · validez |
|-------|----------|----------------------:|----------------------:|
| STRIPS (13) | complete | 0.937 · 87 % | 0.958 · 92 % |
| STRIPS (13) | missing30 | 0.916 · 80 % | 0.958 · 92 % |
| STRIPS (13) | noise5 | 0.574 · 2 % | 0.422 · 1 % |
| numéricos (2) | complete | 0.749 · 51 % | 0.820 · 100 % |
| numéricos (2) | missing30 | 0.704 · 53 % | 0.815 · 100 % |
| numéricos (2) | noise5 | 0.359 · 4 % | 0.522 · 51 % |

Por dominio, con las trazas completas (F · planes válidos):

| Colección | Original | Mejorada |
|-----------|---------:|---------:|
| `ipc-1998/gripper-round-1-strips` | 0.977 · 27/90 | 1.000 · 90/90 |
| `ipc-2000/blocks-strips-untyped` | 1.000 · 90/90 | 1.000 · 90/90 |
| `ipc-2011/barman-sequential-satisficing` | 0.976 · 89/89 | 0.979 · 89/89 |
| `ipc-2011/elevator-sequential-satisficing` | 0.901 · 82/82 | 0.901 · 82/82 |
| `ipc-2011/floor-tile-sequential-satisficing` | 0.919 · 90/90 | 0.919 · 90/90 |
| `ipc-2011/no-mystery-sequential-satisficing` | 0.909 · 90/90 | 1.000 · 90/90 |
| `ipc-2011/parking-sequential-satisficing` | 0.947 · 90/90 | 0.947 · 90/90 |
| `ipc-2011/peg-solitaire-sequential-satisficing` | 0.952 · 90/90 | 0.984 · 90/90 |
| `ipc-2011/scanalyzer-3d-sequential-satisficing` | 0.979 · 89/89 | 0.979 · 89/89 |
| `ipc-2011/sokoban-sequential-satisficing` | 1.000 · 90/90 | 1.000 · 90/90 |
| `ipc-2011/transport-sequential-satisficing` | 0.965 · 90/90 | 0.986 · 90/90 |
| `ipc-2011/visit-all-sequential-satisficing` | 0.833 · 90/90 | 0.909 · 90/90 |
| `ipc-2011/woodworking-sequential-satisficing` | 0.823 · 0/90 | 0.856 · 0/90 |
| `ipc-2023-numeric/counters` | 0.667 · 72/90 | 0.750 · 90/90 |
| `ipc-2023-numeric/zenotravel` | 0.832 · 19/90 | 0.890 · 90/90 |

- **Woodworking** usa constantes del dominio (`natural`, `untreated`…), que PlanMiner todavía no
  aprende: PyPlanner rechaza los problemas porque el dominio aprendido no las declara.
- **Lo que queda por debajo de F 1** sin ruido es sobre todo ambigüedad de los datos
  (precondiciones que se cumplen siempre en las trazas, ver
  [Cómo aprende PlanMiner](algoritmo.md#límites)) y formas equivalentes que la comparación no
  reconoce: con enteros, `(< (value ?0) (max_int))` es `(<= (+ (value ?0) 1) (max_int))`.
- **Con ruido** casi nada funciona todavía.
