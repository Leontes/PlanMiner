cmake_minimum_required(VERSION 3.25)

project(
  PlanMiner
  VERSION 0.1.0
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
option(PLANMINER_BUILD_LEGACY "Compila Old/ como oráculo de los tests" ${BUILD_TESTING})

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo de compilación" FORCE)
endif()

# El estándar se fija por target (cxx_std_20) y no de forma global, para que Old/ siga
# compilándose con el suyo.
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CompilerWarnings)
include(Sanitizers)

add_subdirectory(src)
add_subdirectory(apps)

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
