# Ejecuta el PlanMiner original (Old/) sobre una traza y compara el dominio aprendido con el
# de referencia.
#
#   cmake -DLEGACY=<ejecutable> -DTRACE=<traza.pts> -DDOMAIN_NAME=<nombre>
#         -DEXPECTED=<referencia.pddl> -DWORK_DIR=<directorio> -P RunLegacy.cmake
#
# Con la variable de entorno PLANMINER_UPDATE_GOLDEN definida, en vez de comparar, copia el
# dominio aprendido sobre el de referencia.

foreach(variable IN ITEMS LEGACY TRACE DOMAIN_NAME EXPECTED WORK_DIR)
  if(NOT DEFINED ${variable})
    message(FATAL_ERROR "Falta -D${variable}")
  endif()
endforeach()

# El original escribe en el directorio de trabajo: outputPM/, traza.txt y <nombre>.pddl.
file(REMOVE_RECURSE "${WORK_DIR}")
file(MAKE_DIRECTORY "${WORK_DIR}")

execute_process(
  COMMAND "${LEGACY}" "${TRACE}" "${DOMAIN_NAME}"
  WORKING_DIRECTORY "${WORK_DIR}"
  OUTPUT_FILE "${WORK_DIR}/stdout.log"
  ERROR_FILE "${WORK_DIR}/stderr.log"
  RESULT_VARIABLE result)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "El PlanMiner original ha terminado con '${result}' (salidas en ${WORK_DIR})")
endif()

set(learned "${WORK_DIR}/${DOMAIN_NAME}.pddl")

if(DEFINED ENV{PLANMINER_UPDATE_GOLDEN})
  file(COPY_FILE "${learned}" "${EXPECTED}")
  message(STATUS "Actualizado ${EXPECTED}")
  return()
endif()

execute_process(COMMAND "${CMAKE_COMMAND}" -E compare_files "${learned}" "${EXPECTED}"
                RESULT_VARIABLE different)
if(different)
  message(FATAL_ERROR "El dominio aprendido no coincide con el de referencia:\n"
                      "  aprendido:  ${learned}\n  referencia: ${EXPECTED}")
endif()
