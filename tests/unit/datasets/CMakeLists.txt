add_executable(datasets_tests test_effects.cpp test_relations.cpp test_table.cpp)
target_link_libraries(datasets_tests PRIVATE datasets planminer_test_main)
target_compile_definitions(datasets_tests
                           PRIVATE PLANMINER_TRACES_DIR="${PROJECT_SOURCE_DIR}/tests/data/traces")
planminer_set_warnings(datasets_tests)
planminer_enable_sanitizers(datasets_tests)

gtest_discover_tests(datasets_tests TEST_PREFIX "unit.datasets." DISCOVERY_MODE PRE_TEST
                     NO_PRETTY_VALUES PROPERTIES LABELS unit)
