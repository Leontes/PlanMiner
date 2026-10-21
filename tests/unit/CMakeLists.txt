add_executable(planminer_unit_tests test_version.cpp)
target_link_libraries(planminer_unit_tests PRIVATE planminer::core GTest::gtest_main)
planminer_set_warnings(planminer_unit_tests)
planminer_enable_sanitizers(planminer_unit_tests)

gtest_discover_tests(planminer_unit_tests TEST_PREFIX "unit." DISCOVERY_MODE PRE_TEST
                     NO_PRETTY_VALUES PROPERTIES LABELS unit)

# La línea de comandos.
add_test(NAME unit.cli.version COMMAND planminer --version)
set_tests_properties(unit.cli.version PROPERTIES LABELS unit PASS_REGULAR_EXPRESSION
                                                 "^planminer ${PROJECT_VERSION} \\(")

add_test(NAME unit.cli.unknown_option COMMAND planminer --no-existe)
set_tests_properties(unit.cli.unknown_option PROPERTIES LABELS unit WILL_FAIL ON)
