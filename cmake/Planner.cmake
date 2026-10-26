# Herramientas de los tests del planificador: PyPlanner, pddldomains y VAL.
#
# Los tests del planificador (etiqueta planner) comprueban el arnés que valida dominios
# aprendidos (PyPlanner busca planes con ellos y VAL los valida con el dominio de referencia), que
# las trazas de tests/data/traces se regeneran igual (pddldomains) y el banco de pruebas mínimo
# (benchmarks/smoke.toml). Se registran solo si están las herramientas:
#
# - PLANMINER_PYTHON: intérprete de Python con PyPlanner y pddldomains. Por defecto, el de
#   .venv (scripts/setup_python.sh).
# - PLANMINER_VAL: el ejecutable Validate de VAL. Por defecto, tools/bin/Validate
#   (scripts/build_val.sh).
#
# Tras instalarlas hay que volver a configurar.

set(PLANMINER_PYTHON "" CACHE FILEPATH "Intérprete de Python con PyPlanner y pddldomains")
if(NOT PLANMINER_PYTHON AND EXISTS "${PROJECT_SOURCE_DIR}/.venv/bin/python3")
  set(PLANMINER_PYTHON "${PROJECT_SOURCE_DIR}/.venv/bin/python3" CACHE FILEPATH
      "Intérprete de Python con PyPlanner y pddldomains" FORCE)
endif()

find_program(PLANMINER_VAL NAMES Validate HINTS "${PROJECT_SOURCE_DIR}/tools/bin"
             DOC "Validate de VAL")

set(PLANMINER_PLANNER_TESTS OFF)
set(_planner_missing "")

if(PLANMINER_PYTHON)
  execute_process(
    COMMAND "${PLANMINER_PYTHON}" -c "import pyplanner; print(pyplanner.__version__)"
    RESULT_VARIABLE _pyplanner_result
    OUTPUT_VARIABLE PLANMINER_PYPLANNER_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
  if(NOT _pyplanner_result EQUAL 0)
    list(APPEND _planner_missing "PyPlanner en ${PLANMINER_PYTHON}")
  endif()
  execute_process(
    COMMAND "${PLANMINER_PYTHON}" -c "import pddldomains; print(pddldomains.__version__)"
    RESULT_VARIABLE _pddldomains_result
    OUTPUT_VARIABLE PLANMINER_PDDLDOMAINS_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
  if(NOT _pddldomains_result EQUAL 0)
    list(APPEND _planner_missing "pddldomains en ${PLANMINER_PYTHON}")
  endif()
else()
  list(APPEND _planner_missing "PyPlanner y pddldomains (scripts/setup_python.sh)")
endif()

if(NOT PLANMINER_VAL)
  list(APPEND _planner_missing "VAL (scripts/build_val.sh)")
endif()

if(_planner_missing)
  list(JOIN _planner_missing ", " _planner_missing)
  message(STATUS "Tests del planificador desactivados: falta ${_planner_missing}")
else()
  set(PLANMINER_PLANNER_TESTS ON)
  message(STATUS "Tests del planificador: PyPlanner ${PLANMINER_PYPLANNER_VERSION}, "
                 "pddldomains ${PLANMINER_PDDLDOMAINS_VERSION}, VAL en ${PLANMINER_VAL}")
endif()
