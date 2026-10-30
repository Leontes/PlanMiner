add_executable(pddl_tests test_domain.cpp test_simplify.cpp)
target_link_libraries(pddl_tests PRIVATE pddl planminer_test_main)
target_compile_definitions(pddl_tests
                           PRIVATE PLANMINER_TRACES_DIR="${PROJECT_SOURCE_DIR}/tests/data/traces")
planminer_set_warnings(pddl_tests)
planminer_enable_sanitizers(pddl_tests)

gtest_discover_tests(pddl_tests TEST_PREFIX "unit.pddl." DISCOVERY_MODE PRE_TEST NO_PRETTY_VALUES
                     PROPERTIES LABELS unit)
