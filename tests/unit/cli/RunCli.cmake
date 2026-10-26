# Ejecuta la línea de comandos y comprueba su código de salida y lo que escribe.
#
#   cmake "-DCOMMAND=<planminer>;<argumentos>" -DEXIT=<código> [-DOUTPUT=<regex>]
#         [-DERROR=<regex>] [-DFILE=<fichero> -DCONTENT=<regex>] -P RunCli.cmake

foreach(variable IN ITEMS COMMAND EXIT)
  if(NOT DEFINED ${variable})
    message(FATAL_ERROR "Falta -D${variable}")
  endif()
endforeach()

if(DEFINED FILE)
  file(REMOVE "${FILE}")
endif()

execute_process(
  COMMAND ${COMMAND}
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output
  ERROR_VARIABLE error)

if(NOT result EQUAL EXIT)
  message(FATAL_ERROR "Código de salida ${result}, se esperaba ${EXIT}.\n${output}\n${error}")
endif()
if(DEFINED OUTPUT AND NOT output MATCHES "${OUTPUT}")
  message(FATAL_ERROR "La salida no contiene '${OUTPUT}':\n${output}")
endif()
if(DEFINED ERROR AND NOT error MATCHES "${ERROR}")
  message(FATAL_ERROR "Los errores no contienen '${ERROR}':\n${error}")
endif()
if(DEFINED FILE)
  if(NOT EXISTS "${FILE}")
    message(FATAL_ERROR "No se ha escrito ${FILE}")
  endif()
  file(READ "${FILE}" content)
  if(NOT content MATCHES "${CONTENT}")
    message(FATAL_ERROR "${FILE} no contiene '${CONTENT}':\n${content}")
  endif()
endif()
