# Hallazgos sobre INSLV

Lo que se ha encontrado sobre INSLV al usarlo en la reescritura de PlanMiner (octubre de 2026).
Es el punto de partida para portarlo y arreglarlo en su propio repositorio. Nada de esto se ha
corregido aquí: INSLV queda fuera de la reescritura, y corregir el aprendizaje cambiaría las
reglas respecto al original.

Las referencias son a esta copia (`src/`). Los cambios que ya se hicieron respecto a `Old/` están
en el [README](README.md#cambios-respecto-a-old).

## Estado

- **Tests:** pasan los 32: 6 de la interfaz (`tests/test_learn.cpp`) y los 26 casos de
  equivalencia con el original (`tests/test_equivalence.cpp`).
- **Sanitizers:** con INSLV compilado con AddressSanitizer y UBSan, los 32 tests no dan ningún
  error de memoria ni comportamiento indefinido. Solo fugas de memoria (siguiente apartado).
- **Tiempo:** cada llamada a `learn` tarda como mucho 1 s con los casos de equivalencia (de 230 a
  1732 ejemplos y de 2 a 17 atributos), compilado con `-O2`.
- **Tamaño:** unas 21 200 líneas entre fuentes y cabeceras; `RuleCreator.cpp` tiene más de 10 000.

## Fallos

### Fugas de memoria

Unos 800 bytes por llamada: 25 674 bytes en las 33 llamadas a `learn` de los tests. Tres causas:

- **`multipoblacion::~multipoblacion`** (`multipoblacion.cpp:209-213`) libera las filas de
  `valoracion` recorriendo `n_valoracion`, pero el constructor reserva `n_individuos` filas
  (`multipoblacion.cpp:59-63`). Se pierden las que sobran: son casi todas las fugas (unos 728
  bytes por llamada). Si alguna vez hubiera menos individuos que valoraciones, liberaría filas
  que no existen. Con los parámetros de `Aprender` (`Indiv = rango[0]*10`) no pasa.
- **`poblacionreal::PoblacionInicial`** (`poblacionreal.cpp:390`) reserva `max` y `min` y no los
  libera.
- **`example_set`:** el constructor reserva sus vectores aunque haya 0 ejemplos (`new T[0]`,
  `example_set.cpp:28-46`), pero el destructor y `operator=` solo los liberan si hay ejemplos
  (`example_set.cpp:223` y `270-283`). Se pierden siempre con el conjunto de test que saca
  `Aprender` (`RuleCreator.cpp:9502`): está vacío, porque todos los ejemplos van a la misma
  partición.

En PlanMiner, el preset `asan` las suprime (`tests/lsan.supp`, `leak:inslv::`) para poder probar
con sanitizers las librerías que usan INSLV.

### Indentación engañosa en las funciones de bondad

En las tres funciones de bondad (`RuleCreator.cpp:4035`, `4582` y `9236`):

```cpp
if (positivos-negativos<0)
  valor[0]=-0.9;
else
  //valor[0]=...  (alternativas comentadas)
  valor[0]=positivos-negativos;
  valor[0] *= peso * peso * peso * peso * peso * peso * peso * peso * peso;
```

La multiplicación por `peso^9` está sangrada como parte del `else`, pero se ejecuta siempre,
también cuando `valor[0]` es -0,9. No se ha comprobado qué quería el autor: las alternativas
comentadas sugieren que el `else` se editó varias veces. Si se corrige, cambian las reglas y hay
que regenerar los casos de equivalencia.

### Otros avisos sospechosos, pero correctos

Con `-Wall -Wextra` estos parecen fallos y no lo son:

- `multipoblacion.cpp:1545`: `modificado[i]=...` va sangrado dentro del `for` anterior, pero se
  ejecuta una vez, que es lo que hace falta.
- `RuleCreator.cpp:8724`: el `else` colgante se asocia al `if (max==0)` interior, como parece
  querer.
- `multipoblacion.cpp:1651`: `&&` dentro de `||` sin paréntesis; la precedencia da el desempate
  que se busca.
- `RuleCreator.cpp:7699`: la copia implícita de `nodo` está obsoleta (`-Wdeprecated-copy`).

### Más avisos

418 avisos con `-Wall -Wextra`: 303 de variables sin usar, 47 de parámetros sin usar, 40 de
variables que se asignan y no se usan, 20 de comparaciones entre enteros con y sin signo, y los
siete del apartado anterior.

### Errores que terminan el programa

Hay 23 llamadas a `exit(1)`: 7 en `RuleCreator.cpp`, 5 en `domain_t.cpp`, 4 en
`variable_t.cpp`, 4 en `VectorVar.cpp`, 2 en `example_set.cpp` y 1 en `Relaciones.cpp`. Una
librería debería lanzar una excepción. `learn` lanza `std::invalid_argument` si los datos no
son coherentes (tamaños, variables sin etiquetas, clases fuera de rango), pero no se ha
comprobado si eso evita todos los `exit(1)`; los demás errores internos siguen terminando el
programa.

### Tildes perdidas

104 líneas de comentarios tienen el carácter de reemplazo (U+FFFD) donde había una tilde
(`Operaci�n`). Ya estaban así en `Old/`: se perdieron antes de la reescritura. No hay ninguno
en cadenas, solo en comentarios.

## El generador aleatorio global

`learn` hace `srand(seed)` al empezar y después usa `rand()`: reinicia el generador del programa
que lo llama y lo deja en un estado que depende de los datos.

En PlanMiner esto se nota. La discretización del original resiembra con `rand()` los clusters
vacíos de k-means, y discretiza los ejemplos numéricos después de aprender las reglas de los
lógicos. Para reproducir las particiones del original, PlanMiner tiene que hacer los dos pasos en
ese orden y discretizar con el `rand()` global.

Al portarlo, un generador propio (en `Options`) evitaría interferir con el programa y permitiría
llamar a `learn` desde varios hilos. Pero cambia la secuencia aleatoria y, con ella, las reglas:
habría que regenerar los casos de equivalencia o mantener un modo que reproduzca `rand()` de
glibc. Lo mismo pasa con la desviación de `std::cout`, que tampoco admite varios hilos.

## Cobertura de los tests

- **La parte incremental no se prueba.** `learn` procesa los ejemplos en episodios de 5000, y
  ningún caso tiene más de 1732 ejemplos: los tests solo pasan por un episodio.
- **Los casos vienen de dos dominios**, blocksworld y depots, con las trazas de `Old/Test`.
  PlanMiner comprueba además el pipeline completo hasta INSLV con counters
  (`tests/oracle/test_legacy_rules.cpp` en PlanMiner), con las mismas reglas que el original.

## Cómo lo usa PlanMiner

Lo que conviene tener en cuenta al diseñar la interfaz del repositorio propio:

- **Dos conjuntos de ejemplos por acción.** El lógico tiene los átomos (etiquetas `False` y
  `True`). El numérico tiene los valores de los fluentes y del instante (etiquetas `L_i` de la
  discretización), las relaciones entre fluentes (`False` y `True`) y el signo del cambio de
  cada fluente (`NA`, `POS` y `NEG`). La clase es `PreState` o `PostState`.
- **Las particiones de la discretización del original llegan sin ordenar por valor**, en el
  orden de k-means, y con trapecios que se solapan. No se ha encontrado en INSLV nada que suponga
  etiquetas ordenadas, pero no se ha revisado a fondo.
- **Un valor desconocido (NaN) se adapta por completo a todas las etiquetas.**
- **La interpretación de las reglas está fuera** (en PlanMiner, `rules::learn`; en el original,
  `ruleset::SplitAntecedent`). Solo cuenta los atributos de hasta dos etiquetas, con el nombre de
  la etiqueta o su centro truncado a entero. Así se pierden los atributos numéricos de más de
  dos etiquetas y el cambio de un fluente que sube y baja (`NA`, `POS` y `NEG`). En el original,
  además, la fusión de reglas descartaba las condiciones sobre valores numéricos de dos
  etiquetas: lo que aprende INSLV sobre los valores discretizados nunca llega al dominio.
  `learn` ya devuelve las reglas completas (atributos y etiquetas), así que esto es cosa de quien
  las interpreta.

## Casos de equivalencia

Los 26 casos de `tests/cases/` son autosuficientes: la entrada exacta de `inslv()` en el
original y las reglas que devolvió, ya interpretadas. Se generaron con
`tests/oracle/dump_inslv_cases.cpp` de PlanMiner, que necesita `Old/`, y `Old/` ya solo está en
local (sigue en el historial de PlanMiner). Al mover INSLV a su repositorio hay que llevarse los
casos tal cual: no se podrán regenerar fuera de PlanMiner.

## Cómo reproducir lo anterior

```bash
# Tests con AddressSanitizer y UBSan en el propio INSLV (con límite de memoria)
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer"
cmake --build build-asan -j 2
ASAN_OPTIONS=hard_rss_limit_mb=2048 ./build-asan/tests/inslv_tests

# Recuento de avisos
for f in src/*.cpp; do g++ -std=c++17 -fsyntax-only -Wall -Wextra -Iinclude -Isrc "$f"; done 2>&1 \
  | grep -oE '\[-W[a-z0-9=-]+\]' | sort | uniq -c | sort -rn
```
