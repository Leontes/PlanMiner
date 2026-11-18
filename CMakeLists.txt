cmake_minimum_required(VERSION 3.25)

project(
  PlanMiner
  VERSION 1.1.0
  DESCRIPTION "Aprendizaje de dominios de planificación a partir de trazas de planes"
  HOMEPAGE_URL "https://github.com/Leontes/PlanMiner"
  LANGUAGES CXX)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "Compila fuera del código fuente, por ejemplo: cmake --preset debug")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Opciones
option(BUILD_TESTING "Compila los tests" ON)
option(PLANMINER_WARNINGS_AS_ERRORS "Trata los avisos del código nuevo como errores" OFF)
option(PLANMINER_SANITIZERS "Compila el código nuevo con AddressSanitizer y UBSan" OFF)
# Old/, la implementación original, solo está en local (no se sube): el oráculo de los tests se
# compila si está.
if(BUILD_TESTING AND EXISTS "${PROJECT_SOURCE_DIR}/Old/CMakeLists.txt")
  set(_legacy_default ON)
else()
  set(_legacy_default OFF)
endif()
option(PLANMINER_BUILD_LEGACY "Compila Old/ como oráculo de los tests" ${_legacy_default})

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo de compilación" FORCE)
endif()

# El estándar se fija por target (cxx_std_20) y no de forma global, para que Old/ siga
# compilándose con el suyo.
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CompilerWarnings)
include(Sanitizers)
include(Libraries)
include(Json)
include(Inslv)

if(BUILD_TESTING)
  enable_testing()
  include(Dependencies)
endif()

add_subdirectory(libs)
add_subdirectory(apps)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
