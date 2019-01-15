cmake_minimum_required (VERSION 3.1)

project (PlanMiner)


#####################
# Sources directories
file(GLOB SOURCES "src/*.cpp" "src/*.cxx" "src/*.cc")
file(GLOB SOURCES_LIBTASK "src/Task/*.cpp" "src/Task/*.cxx" "src/Task/*.cc")
file(GLOB SOURCES_LIBSTATE "src/State/*.cpp" "src/State/*.cxx" "src/State/*.cc")
file(GLOB SOURCES_INSLV "src/INSLV/*.cpp" "src/INSLV/*.cxx" "src/INSLV/*.cc")


#####################
# Headers directories
include_directories("include")


###########
# Set flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


###################
# Compile libraries
add_library(taskLib ${SOURCES_LIBTASK})
target_link_libraries(taskLib)

add_library(stateLib ${SOURCES_LIBSTATE})
target_link_libraries(stateLib)

add_library(inslvLib ${SOURCES_INSLV})
target_link_libraries(inslvLib)

######################
# Compile Main program
add_executable(${PROJECT_NAME} "src/bin/mainPlanMiner.cpp" ${SOURCES})
target_link_libraries(${PROJECT_NAME} taskLib stateLib inslvLib)
