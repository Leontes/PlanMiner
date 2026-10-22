# Los tests unitarios de cada librería de libs/, en tests/unit/<librería>, con el prefijo
# unit.<librería>.
add_subdirectory(plantraces)
add_subdirectory(datasets)

# La línea de comandos.
add_test(NAME unit.cli.version COMMAND planminer --version)
set_tests_properties(unit.cli.version PROPERTIES LABELS unit PASS_REGULAR_EXPRESSION
                                                 "^planminer ${PROJECT_VERSION} \\(")

add_test(NAME unit.cli.unknown_option COMMAND planminer --no-existe)
set_tests_properties(unit.cli.unknown_option PROPERTIES LABELS unit WILL_FAIL ON)
