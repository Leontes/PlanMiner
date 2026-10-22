add_executable(resources_tests test_memory.cpp)
target_link_libraries(resources_tests PRIVATE resources planminer_test_main)
planminer_set_warnings(resources_tests)
planminer_enable_sanitizers(resources_tests)

gtest_discover_tests(resources_tests TEST_PREFIX "unit.resources." DISCOVERY_MODE PRE_TEST
                     NO_PRETTY_VALUES PROPERTIES LABELS unit)
