add_executable(plantraces_tests test_trace.cpp test_trace_dataset.cpp)
target_link_libraries(plantraces_tests PRIVATE plantraces nlohmann_json::nlohmann_json
                                               GTest::gtest_main)
target_compile_definitions(plantraces_tests
                           PRIVATE PLANMINER_TRACES_DIR="${PROJECT_SOURCE_DIR}/tests/data/traces")
planminer_set_warnings(plantraces_tests)
planminer_enable_sanitizers(plantraces_tests)

gtest_discover_tests(plantraces_tests TEST_PREFIX "unit.plantraces." DISCOVERY_MODE PRE_TEST
                     NO_PRETTY_VALUES PROPERTIES LABELS unit)
