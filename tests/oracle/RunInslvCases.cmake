# Genera con Old/ los casos de equivalencia de INSLV y los compara con los guardados en
# vendor/inslv/tests/cases.
#
#   cmake -DGENERATOR=<dump_inslv_cases> -DTRACES=<prefijo>=<traza.pts>;...
#         -DEXPECTED_DIR=<casos guardados> -DWORK_DIR=<directorio>
#         [-DLIMIT=<planminer_limit_memory> -DMEMORY_MB=<MB>] -P RunInslvCases.cmake
#
# Con LIMIT y MEMORY_MB, el generador se ejecuta con ese límite duro de memoria.
#
# Con la variable de entorno PLANMINER_UPDATE_GOLDEN definida, en vez de comparar, sustituye los
# casos guardados por los generados.

foreach(variable IN ITEMS GENERATOR TRACES EXPECTED_DIR WORK_DIR)
  if(NOT DEFINED ${variable})
    message(FATAL_ERROR "Falta -D${variable}")
  endif()
endforeach()

file(REMOVE_RECURSE "${WORK_DIR}")
file(MAKE_DIRECTORY "${WORK_DIR}/run")

set(launcher "")
if(DEFINED LIMIT AND DEFINED MEMORY_MB)
  set(launcher "${LIMIT}" "${MEMORY_MB}")
endif()

execute_process(
  COMMAND ${launcher} "${GENERATOR}" "${WORK_DIR}/cases" ${TRACES}
  WORKING_DIRECTORY "${WORK_DIR}/run"
  OUTPUT_FILE "${WORK_DIR}/stdout.log"
  ERROR_FILE "${WORK_DIR}/stderr.log"
  RESULT_VARIABLE result)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "El generador ha terminado con '${result}' (salidas en ${WORK_DIR})")
endif()

file(GLOB generated RELATIVE "${WORK_DIR}/cases" "${WORK_DIR}/cases/*.case")
file(GLOB expected RELATIVE "${EXPECTED_DIR}" "${EXPECTED_DIR}/*.case")

if(DEFINED ENV{PLANMINER_UPDATE_GOLDEN})
  foreach(case IN LISTS expected)
    file(REMOVE "${EXPECTED_DIR}/${case}")
  endforeach()
  foreach(case IN LISTS generated)
    file(COPY_FILE "${WORK_DIR}/cases/${case}" "${EXPECTED_DIR}/${case}")
  endforeach()
  list(LENGTH generated count)
  message(STATUS "Actualizados ${count} casos en ${EXPECTED_DIR}")
  return()
endif()

if(NOT generated STREQUAL expected)
  message(FATAL_ERROR "Los casos generados no son los guardados:\n"
                      "  generados: ${generated}\n  guardados:  ${expected}")
endif()
foreach(case IN LISTS generated)
  execute_process(COMMAND "${CMAKE_COMMAND}" -E compare_files "${WORK_DIR}/cases/${case}"
                          "${EXPECTED_DIR}/${case}" RESULT_VARIABLE different)
  if(different)
    message(FATAL_ERROR "El caso ${case} no coincide con el guardado (generado en ${WORK_DIR})")
  endif()
endforeach()
