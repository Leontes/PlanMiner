# Herramientas de los tests del planificador: PyPlanner y VAL.
#
# Los tests del planificador (etiqueta planner) comprueban los dominios aprendidos: PyPlanner
# busca planes con ellos y VAL los valida con el dominio de referencia. Se registran solo si
# están las dos herramientas:
#
# - PLANMINER_PYTHON: intérprete de Python con PyPlanner. Por defecto, el de .venv
#   (scripts/setup_python.sh).
# - PLANMINER_VAL: el ejecutable Validate de VAL. Por defecto, tools/bin/Validate
#   (scripts/build_val.sh).
#
# Tras instalarlas hay que volver a configurar.

set(PLANMINER_PYTHON "" CACHE FILEPATH "Intérprete de Python con PyPlanner")
if(NOT PLANMINER_PYTHON AND EXISTS "${PROJECT_SOURCE_DIR}/.venv/bin/python3")
  set(PLANMINER_PYTHON "${PROJECT_SOURCE_DIR}/.venv/bin/python3" CACHE FILEPATH
      "Intérprete de Python con PyPlanner" FORCE)
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
else()
  list(APPEND _planner_missing "PyPlanner (scripts/setup_python.sh)")
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
                 "VAL en ${PLANMINER_VAL}")
endif()
