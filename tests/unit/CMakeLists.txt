# Los tests unitarios de cada librería de libs/, en tests/unit/<librería>, con el prefijo
# unit.<librería>.
add_subdirectory(resources)
add_subdirectory(plantraces)
add_subdirectory(regression)
add_subdirectory(datasets)
add_subdirectory(rules)
add_subdirectory(pddl)

# La línea de comandos: el código de salida y lo que escribe (cli/RunCli.cmake).
set(traces "${PROJECT_SOURCE_DIR}/tests/data/traces")
file(GLOB blocksworld_traces "${traces}/blocks-strips-untyped/*.complete.json")

function(planminer_add_cli_test name exit)
  cmake_parse_arguments(PARSE_ARGV 2 cli "" "OUTPUT;ERROR;FILE;CONTENT" "ARGS")
  set(script_args "-DEXIT=${exit}")
  foreach(check IN ITEMS OUTPUT ERROR FILE CONTENT)
    if(DEFINED cli_${check})
      list(APPEND script_args "-D${check}=${cli_${check}}")
    endif()
  endforeach()
  list(JOIN cli_ARGS ";" arguments)
  add_test(NAME unit.cli.${name}
           COMMAND "${CMAKE_COMMAND}" "-DCOMMAND=$<TARGET_FILE:planminer>;${arguments}"
                   ${script_args} -P "${CMAKE_CURRENT_SOURCE_DIR}/cli/RunCli.cmake")
  set_tests_properties(unit.cli.${name} PROPERTIES LABELS unit TIMEOUT 300)
endfunction()

planminer_add_cli_test(version 0 ARGS --version OUTPUT "^planminer ${PROJECT_VERSION} \\(")
planminer_add_cli_test(help 0 ARGS --help OUTPUT "^uso: planminer")

# Errores de uso: 2.
planminer_add_cli_test(no_traces 2 ERROR "faltan las trazas")
planminer_add_cli_test(unknown_option 2 ARGS --no-existe ERROR "opción no reconocida: --no-existe")
planminer_add_cli_test(missing_value 2 ARGS -o ERROR "falta el valor de -o")
planminer_add_cli_test(bad_memory_limit 2 ARGS -m nada ${blocksworld_traces}
                       ERROR "límite de memoria no válido: nada")

# Errores con las trazas: 1.
planminer_add_cli_test(missing_trace 1 ARGS "${traces}/no-existe.json" ERROR "^error: ")
planminer_add_cli_test(
  mixed_domains 1 ARGS "${traces}/blocks-strips-untyped/p01.complete.json"
  "${traces}/counters/p01.complete.json" ERROR "dominios distintos: blocks y fn-counters")

# El dominio, con el nombre de las trazas o el que se le dé, en la salida o en un fichero.
planminer_add_cli_test(learns_blocksworld 0 ARGS ${blocksworld_traces}
                       OUTPUT "\\(define \\(domain blocks\\).*\\(:action stack")
planminer_add_cli_test(
  writes_the_domain 0 ARGS -n Bloques -m 2048 -o "${CMAKE_CURRENT_BINARY_DIR}/bloques.pddl"
  ${blocksworld_traces} FILE "${CMAKE_CURRENT_BINARY_DIR}/bloques.pddl"
  CONTENT "^\\(define \\(domain bloques\\)")
