cmake_minimum_required (VERSION 3.1)

project (PlanMiner)

##################################
# Download and install GoogleTest

include(ExternalProject)
ExternalProject_Add(gtest
  GIT_REPOSITORY https://github.com/google/googletest
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
  INSTALL_COMMAND ""
)
ExternalProject_Get_Property(gtest source_dir binary_dir)


#############
#Doxigen conf
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)


#####################
# Sources directories
file(GLOB SOURCES "src/*.cpp" "src/*.cxx" "src/*.cc")

#####################
# Headers directories
include_directories("include")
include_directories("gtest/src/gtest/googletest/include")


###########
# Set flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(${PROJECT_NAME} "src/bin/main.cpp" ${SOURCES})
target_link_libraries(${PROJECT_NAME})


################
# Define a test
add_executable(data_tests "src/gtest/data_tests.cpp")

######################################
# Configure the test to use GoogleTest
add_dependencies(data_tests gtest)
target_link_libraries(data_tests ${binary_dir}/googlemock/gtest/libgtest.a ${binary_dir}/googlemock/gtest/libgtest_main.a)


##################################
# Just make the test runnable with
#   $ make test
enable_testing()
add_test(NAME    data_tests
         COMMAND data_tests)
