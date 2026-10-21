cmake_minimum_required(VERSION 3.25)

project(
  INSLV
  DESCRIPTION "Aprendizaje de reglas difusas con NSLV, en su versión incremental"
  LANGUAGES CXX)

option(INSLV_BUILD_TESTS "Compila los tests de INSLV" ${PROJECT_IS_TOP_LEVEL})

add_library(
  inslv STATIC
  src/inslv.cpp
  src/RuleCreator.cpp
  src/Relaciones.cpp
  src/VectorVar.cpp
  src/domain_t.cpp
  src/example_set.cpp
  src/fuzzy_t.cpp
  src/genetcode.cpp
  src/multipoblacion.cpp
  src/operaciones.cpp
  src/poblacionbinaria.cpp
  src/poblacionentera.cpp
  src/poblacionreal.cpp
  src/ruleset.cpp
  src/variable_t.cpp
  src/vectordouble.cpp)
add_library(inslv::inslv ALIAS inslv)

target_include_directories(
  inslv
  PUBLIC "${PROJECT_SOURCE_DIR}/include"
  PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_compile_features(inslv PUBLIC cxx_std_17)

if(INSLV_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
