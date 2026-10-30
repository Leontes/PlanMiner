# Cómo aprende PlanMiner

[English](../en/algorithm.md) · **Español**

PlanMiner aprende cada acción por separado, a partir de sus aplicaciones en las trazas: el estado
de antes de cada una, el de después y sus argumentos. Este documento describe las partes del
algoritmo y lo que cambia la configuración mejorada respecto al PlanMiner original.

## Las partes

1. **Las trazas** (`libs/plantraces`). PlanMiner lee las trazas (`plantraces::read_trace`) y
   agrupa las aplicaciones de cada acción. Un átomo que no está entre los ciertos ni entre los
   desconocidos es falso, también en mundo abierto. La jerarquía de tipos se infiere de los
   objetos de las trazas: si un mismo parámetro aparece con dos tipos, se les da un padre común.
2. **La tabla de ejemplos** (`datasets::build_table`). Para cada acción, una fila por estado (los
   de antes de cada aplicación y, detrás, los de después) y una columna por cada átomo y cada
   fluente que se puede expresar con sus parámetros: `(on ?0 ?1)`, `(fuel ?0)`. Un objeto que
   aparece en varios parámetros se expresa con el primero.
3. **Los efectos numéricos** (`datasets::numeric_effects`, `libs/regression`). Para cada fluente
   que cambia, una búsqueda A* de la expresión aritmética de su cambio, con los demás fluentes y
   constantes, hasta que el error (MAPE) baja del 2 %: `(* (distance ?1 ?2) (slow-burn ?0))`.
4. **Las relaciones numéricas** (`datasets::numeric_relations`). Comparaciones (`=`, `<`, `>`)
   entre fluentes y con las expresiones de los efectos, que se añaden a la tabla como atributos
   ciertos o falsos. De ellas salen las precondiciones numéricas.
5. **Las reglas** (`libs/rules`, `vendor/inslv`). Dos conjuntos de ejemplos por acción: el lógico,
   con los átomos, y el numérico, con los valores de los fluentes (discretizados) y las
   relaciones. La clase de cada fila es estado de antes o de después. INSLV aprende reglas difusas
   que describen cada clase.
6. **La fusión** (`rules::fuse`). Las condiciones de las reglas del estado de antes forman la
   precondición, y las del de después que no estaban antes, el efecto.
7. **La escritura** (`pddl::write`). El dominio en PDDL, con los tipos, los predicados, las
   funciones y las acciones.

`pddl::learn` hace todo el recorrido; con `pddl::Options` se elige cada mejora.

## La configuración mejorada

La configuración original (`--original`, `pddl::Options::original()`) reproduce el PlanMiner
original: con las mismas trazas da el mismo dominio. La mejorada (por defecto, `pddl::Options{}`)
corrige sus fallos y añade lo que le faltaba. Cada mejora es un campo de las opciones, para
poder medirla por separado.

### Efectos numéricos

| Mejora | Qué hace | Opción |
|--------|----------|--------|
| Estado de antes | Las variables de la expresión son los fluentes del estado de antes, incluido el propio (la semántica de PDDL). El original usaba los del estado de después, sin el propio. | `EffectOptions::before_state` |
| Formas de efecto | Se buscan `increase`, `decrease` y `assign`, con su signo. El original buscaba el valor absoluto del cambio y dejaba el signo a INSLV. | `EffectOptions::effect_forms` |
| Modelo nulo | Un fluente que no cambia en al menos el 75 % de las aplicaciones no tiene efecto, aunque el ruido lo altere en alguna. | `EffectOptions::null_model` |
| Plegado de constantes | `(* 10 3)` se escribe `30`. | `EffectOptions::fold_constants` |
| Búsqueda | Sucesores en árbol (el original solo crecía por la derecha y no podía expresar `a * (b + c)`), duplicados por sus valores, constantes del 1 al 10, sin variables constantes y con un tamaño máximo. | `regression::Options` |
| Efectos de la regresión | Los efectos numéricos salen de la regresión, con su forma y su signo. El original le pasaba a INSLV el signo del cambio y convertía en asignaciones condiciones sobre valores discretizados. | `ExampleOptions::changes` |

### Precondiciones numéricas

| Mejora | Qué hace | Opción |
|--------|----------|--------|
| Expresiones de los efectos | Cada fluente se compara con las expresiones de los efectos de la acción: `(>= (fuel ?0) (* (distance ?1 ?2) (slow-burn ?0)))`. | `RelationOptions::effect_expressions` |
| Parejas que cambian | Se compara cada pareja en la que cambia algún lado (con el modelo nulo), en cualquier orden. | `RelationOptions::changing_pairs` |
| Constantes | Se descartan las relaciones que valen lo mismo en todas las filas en que se conocen. | `RelationOptions::known_constants` |
| Sin el instante | El instante no es un fluente: ni se compara ni es un atributo. | `RelationOptions::time`, `ExampleOptions::time` |
| Sin costes ni acumuladores | `(total-cost)` y los fluentes que no bajan nunca (como `total-fuel-used`) no se comparan: sus relaciones dependen de cuándo se aplica la acción. | `RelationOptions::action_costs`, `RelationOptions::accumulators` |
| Cotas que no distinguen | Las cotas (`<=`, `>=`) que se cumplen en todos los estados de antes de la acción, se alcanzan en alguno y no son invariantes son precondiciones, aunque INSLV no las elija por valer lo mismo antes y después: `(<= (onboard ?0) (zoom-limit ?0))`. Solo sin ruido. | `Options::contrast` |

### Reglas y fusión

| Mejora | Qué hace | Opción |
|--------|----------|--------|
| Parámetros repetidos | En una aplicación con un objeto en varios parámetros (`move ?from ?to` con `?from = ?to`), los átomos de esos parámetros son desconocidos: un borrado y una adición del mismo átomo se anulan y el efecto no se ve. | `Options::aliasing` |
| Validación | Cada condición de una regla se queda si se cumple en al menos el 95 % de los ejemplos de su estado. | `FusionOptions::validate` |
| Validación exacta | Si las trazas no tienen ruido, la validación exige todos los ejemplos. El ruido se estima con lo que cambia fuera de los argumentos de cada acción (`plantraces::frame_changes`). | `Options::exact_without_noise` |
| Mayoría | En un conflicto entre dos valores de un atributo gana el que cubre más ejemplos. | `FusionOptions::majority` |
| Comparaciones combinadas | Las comparaciones de una pareja, ciertas y falsas, se combinan en una (`<`, `<=`, `=`, `>=`, `>`). | `FusionOptions::combine_comparisons` |
| Precondiciones redundantes | Se quitan las comparaciones que implican las demás condiciones en los estados de las trazas y los átomos equivalentes a otro de la misma relación: `(connected ?1 ?0)` con `(connected ?0 ?1)`. | `Options::drop_implied` |

### Escritura

| Mejora | Qué hace | Opción |
|--------|----------|--------|
| `:numeric-fluents` | El dominio lo declara si tiene funciones. Sin él, algunos planificadores lo rechazan. | `Options::numeric_fluents` |
| Igualdad | Las igualdades se escriben con `=`. El original escribe `==`, que no es PDDL. | `Options::equality` |

## Límites

- **Lenguaje:** STRIPS con tipos y costes de acción, y fluentes numéricos sin acciones
  durativas. Ni ADL (cuantificadores, efectos condicionales, disyunciones), ni constantes del
  dominio, ni acciones durativas.
- **Ruido:** con un 5 % de ruido en las trazas, INSLV pierde literales esenciales y casi ningún
  dominio aprendido da planes válidos.
- **Ambigüedad:** solo hay ejemplos de lo que pasa, no de lo que no puede pasar. Una precondición
  que se cumple en todos los estados de las trazas en que se aplica la acción no se puede
  descartar con ellas: una relación estática (`(reachable-floor ?lift ?floor)`), un invariante
  del estado (`(visited ?from)`, donde está el robot) o una cota que el plan nunca supera. No
  rompen los planes, pero bajan la precisión.
