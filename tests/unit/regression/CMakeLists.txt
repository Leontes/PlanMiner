add_executable(regression_tests test_expression.cpp test_search.cpp)
target_link_libraries(regression_tests PRIVATE regression resources planminer_test_main)
planminer_set_warnings(regression_tests)
planminer_enable_sanitizers(regression_tests)

gtest_discover_tests(regression_tests TEST_PREFIX "unit.regression." DISCOVERY_MODE PRE_TEST
                     NO_PRETTY_VALUES PROPERTIES LABELS unit)
