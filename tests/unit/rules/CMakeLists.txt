add_executable(rules_tests test_examples.cpp test_fusion.cpp)
target_link_libraries(rules_tests PRIVATE rules planminer_test_main)
planminer_set_warnings(rules_tests)
planminer_enable_sanitizers(rules_tests)

gtest_discover_tests(rules_tests TEST_PREFIX "unit.rules." DISCOVERY_MODE PRE_TEST NO_PRETTY_VALUES
                     PROPERTIES LABELS unit)
