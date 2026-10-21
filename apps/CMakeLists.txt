add_executable(planminer planminer/main.cpp)
target_link_libraries(planminer PRIVATE planminer::core)
set_target_properties(planminer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

planminer_set_warnings(planminer)
planminer_enable_sanitizers(planminer)
