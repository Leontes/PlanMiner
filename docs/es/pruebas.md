# Tests

[English](../en/tests.md) · **Español**

Los tests van por etiquetas de CTest:

```bash
ctest --preset debug                  # todos los que estén registrados
ctest --preset debug -L unit          # solo los unitarios
ctest --preset debug -R 'unit.pddl.*' # por nombre
```

| Etiqueta | Qué comprueba | Necesita |
|----------|---------------|----------|
| `unit` | las librerías, la línea de comandos y los scripts de Python | GoogleTest (si no está instalado, CMake lo descarga) |
| `planner` | el arnés que valida dominios aprendidos, que las trazas de prueba se regeneran igual y el banco de pruebas mínimo, de principio a fin | PyPlanner, pddldomains, VAL y el submódulo `vendor/PDDLDomains` |
| `oracle` | la configuración original frente al PlanMiner original (`Old/`), función a función y de principio a fin | `Old/`, solo en local |

Los tests de INSLV se pasan en [su repositorio](https://github.com/Leontes/INSLV-PlanMiner): al
descargarlo, no se compilan.

## Tests del planificador

Validan los dominios aprendidos planificando con ellos: PyPlanner busca un plan con el dominio
aprendido y VAL lo valida con el de referencia (`scripts/evaluate_domain.py`). Solo se registran
si están las herramientas:

```bash
scripts/setup_python.sh   # .venv con PyPlanner (tests/planner/requirements.txt) y pddldomains
scripts/build_val.sh      # VAL en tools/bin/Validate
cmake --preset debug      # volver a configurar para que los detecte
```

- **El arnés:** el dominio de referencia, evaluado contra sí mismo, da planes válidos, y uno
  defectuoso, planes que VAL rechaza. La validez se decide por el texto de VAL («Plan valid»): su
  código de salida es 0 también con algunos planes inválidos.
- **Las trazas de prueba** (`planner.traces_reproducible`): se regeneran igual a partir de sus
  planes.
- **El banco de pruebas mínimo** (`planner.benchmark.*`): genera los datos de
  [`benchmarks/smoke.toml`](../../benchmarks/smoke.toml), que tienen que coincidir con sus
  huellas, hace la validación cruzada con el PlanMiner recién compilado y escribe el informe.

## Las trazas de prueba

[`tests/data/traces`](../../tests/data/traces) tiene las trazas de los tests unitarios y del
oráculo: blocksworld, counters y zenotravel, del catálogo de PDDLDomains, con varias variantes
(completa, mundo cerrado, con huecos, con estados que faltan y con ruido).
[`dataset.toml`](../../tests/data/traces/dataset.toml) las define, y
`scripts/generate_traces.py` las genera (con `--check`, comprueba que se regeneran igual sin
escribir nada). Los planes son de PyPlanner (`gbfs-ff-pref` con un núcleo, que es determinista).

## El oráculo

Mientras se reescribía, cada parte se comparó con el código original, que sigue en el historial de
git pero ya no en el repositorio (`Old/` está en `.gitignore`). Los tests del oráculo solo se
compilan si `Old/` existe (`PLANMINER_BUILD_LEGACY`), así que la integración continua no los
pasa. Para recuperarlo:

```bash
git archive 3823a14 Old | tar -x
```

Comprueban que la configuración original da lo mismo que el original: función a función, de
principio a fin con las trazas de prueba (el dominio aprendido, comparado con el guardado en
`tests/oracle/golden/`) y los casos de equivalencia de INSLV, que están en su repositorio
(`tests/datos/planminer`). Para regenerar lo guardado:

```bash
PLANMINER_UPDATE_GOLDEN=1 ctest --preset debug -R oracle.legacy
# Los casos de INSLV, en una copia local (-DFETCHCONTENT_SOURCE_DIR_INSLV=<directorio>):
PLANMINER_UPDATE_GOLDEN=1 ctest --preset debug -R oracle.inslv_cases
```

## Memoria

- Cada ejecutable de tests se limita a 2048 MB (`PLANMINER_TEST_MEMORY_MB`; 0, sin límite). Con
  `ctest` en paralelo (4 trabajos), el peor caso es 8 GB.
- Los tests lanzan PyPlanner siempre con límite de memoria (`-m`).
- Con AddressSanitizer (preset `asan`), el límite de memoria del proceso no es compatible: se usa
  `ASAN_OPTIONS=hard_rss_limit_mb`.

## Integración continua

[`.github/workflows/ci.yml`](../../.github/workflows/ci.yml):

- compilación y tests con GCC y con Clang (avisos como errores);
- AddressSanitizer y UBSan;
- los tests del planificador, con VAL compilado (en caché) y el entorno de Python;
- el formato del C++ (clang-format 18) y los scripts de Python (ruff y `mypy --strict`).
