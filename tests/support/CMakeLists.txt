# Lo común de los tests: el límite duro de memoria de cada proceso (ver resources/memory.hpp).

# El main de los tests de GoogleTest, con el límite al empezar (PLANMINER_TEST_MEMORY_MB).
add_library(planminer_test_main STATIC gtest_main.cpp)
target_link_libraries(planminer_test_main PUBLIC GTest::gtest PRIVATE resources)
target_compile_features(planminer_test_main PRIVATE cxx_std_20)
planminer_set_warnings(planminer_test_main)
planminer_enable_sanitizers(planminer_test_main)

# El lanzador con límite, para los programas que no son de PlanMiner (el original de Old/).
add_executable(planminer_limit_memory limit_memory.cpp)
target_link_libraries(planminer_limit_memory PRIVATE resources)
target_compile_features(planminer_limit_memory PRIVATE cxx_std_20)
planminer_set_warnings(planminer_limit_memory)
