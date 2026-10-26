configure_file(planminer/version.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/planminer/version.hpp" @ONLY)

add_executable(planminer planminer/main.cpp)
target_link_libraries(planminer PRIVATE pddl plantraces resources)
target_include_directories(planminer PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/planminer")
target_compile_features(planminer PRIVATE cxx_std_20)
set_target_properties(planminer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

planminer_set_warnings(planminer)
planminer_enable_sanitizers(planminer)
