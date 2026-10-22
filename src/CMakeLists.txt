# planminer::core: la librería con todo el algoritmo. La línea de comandos y los tests
# enlazan con ella.

configure_file("${PROJECT_SOURCE_DIR}/include/planminer/version.hpp.in"
               "${PROJECT_BINARY_DIR}/include/planminer/version.hpp" @ONLY)

add_library(planminer_core STATIC trace.cpp version.cpp)
add_library(planminer::core ALIAS planminer_core)

target_include_directories(
  planminer_core PUBLIC "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}/include")
target_compile_features(planminer_core PUBLIC cxx_std_20)
target_link_libraries(planminer_core PRIVATE nlohmann_json::nlohmann_json)
target_compile_definitions(
  planminer_core
  PRIVATE PLANMINER_COMPILER="${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}"
          PLANMINER_BUILD_TYPE="$<CONFIG>")

planminer_set_warnings(planminer_core)
planminer_enable_sanitizers(planminer_core)
