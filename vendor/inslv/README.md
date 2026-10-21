# INSLV

Aprendizaje de reglas difusas con el algoritmo genético de NSLV, en su versión incremental: los
ejemplos se procesan por episodios de 5000.

Está aquí como dependencia de PlanMiner, **fuera del alcance de su reescritura**, hasta que tenga
su propio repositorio. Es un proyecto de CMake independiente: se compila y se prueba por
separado, y PlanMiner lo incluye con `add_subdirectory(vendor/inslv)`.

## Origen

El código es el de `Old/` en PlanMiner: la librería de `Old/src/INSLV` y el algoritmo de
`Old/src/Rules/RuleCreator.cpp`. Parte de NSLV, de Raúl Pérez (2002), según las cabeceras de
los fuentes.

## Uso

```cpp
#include "inslv/inslv.hpp"

inslv::Dataset data;
data.attributes = {{"a", {{"False", 0, 0, 0, 0}, {"True", 1, 1, 1, 1}}}};
data.target = {"Class", {{"Negativo", 0, 0, 0, 0}, {"Positivo", 1, 1, 1, 1}}};
data.values = {{0, 1, 1, 0}};  // values[atributo][ejemplo]; NaN: desconocido
data.classes = {0, 1, 1, 0};   // índice de la etiqueta de target

for (const inslv::Rule& rule : inslv::learn(data)) {
  // rule.antecedent: atributos y etiquetas que admiten; rule.consequent: la clase;
  // rule.covered: ejemplos positivos cubiertos
}
```

`inslv::Options` fija la semilla (por defecto, 0), si se muestra el progreso (`verbose`) y si
se escribe el informe del aprendizaje (`report`).

```cmake
add_subdirectory(vendor/inslv)
target_link_libraries(mi_programa PRIVATE inslv::inslv)
```

## Cambios respecto a Old/

El aprendizaje es el mismo: con los mismos datos y la misma semilla da las mismas reglas (ver
los tests). Lo que cambia:

- **Interfaz:** `inslv::learn` con los tipos de `inslv.hpp`, en lugar de `inslv()` con las
  estructuras de PlanMiner. Las reglas salen completas: la interpretación que hacía PlanMiner
  (`ruleset::SplitAntecedent` y `ruleset::toPDDL`) queda fuera de la librería.
- **Espacio de nombres `inslv`:** se puede enlazar en el mismo programa que `Old/`.
- **Arreglos:**
  - el `return` que faltaba en `multipoblacion::Sort_4L`, `ruleset::operator=` y `Operacion`
    (con los compiladores actuales, el programa fallaba);
  - los vectores de inicio y fin de los episodios se dimensionan con el número de episodios (en
    el original eran dos vectores fijos de 10^8 enteros: 800 MB por llamada);
  - `Bondad` y `Bondad_E` leían fuera de un vector un valor que luego no usaban;
  - `std::random_shuffle`, que ya no existe en C++17, se sustituye por la misma mezcla con
    `rand()`.
- **Efectos secundarios:**
  - el informe es opcional y ya no se escriben `traza.txt` ni otros ficheros en el directorio de
    trabajo, ni se llama a `system("date")`;
  - `Pausa()` ya no espera una tecla;
  - la salida por `std::cout` se silencia salvo con `verbose`.
- Todos los fuentes están en UTF-8.

## Tests

```bash
cmake -S . -B build && cmake --build build && ctest --test-dir build
```

- `tests/test_learn.cpp`: la interfaz.
- `tests/test_equivalence.cpp`: los casos de `tests/cases/`. Cada uno es una llamada a `inslv()`
  en el PlanMiner original, con su entrada y las reglas que devolvió. Se generan con `Old/`,
  desde PlanMiner:

  ```bash
  PLANMINER_UPDATE_GOLDEN=1 ctest --preset debug -R oracle.inslv_cases
  ```

## Limitaciones conocidas

- Usa `srand` y `rand`, y desvía `std::cout`: no se puede llamar a la vez desde varios hilos.
- Algunos errores internos terminan el programa con `exit(1)`.
- Pierde menos de 1 KB de memoria por llamada (copias de `example_set` y `multipoblacion`).
