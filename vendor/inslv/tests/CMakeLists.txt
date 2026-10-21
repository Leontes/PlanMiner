# GoogleTest: el del proyecto que incluye a INSLV si ya lo tiene; si no, el instalado (1.14 o
# posterior) o, si no hay, se descarga.
if(NOT TARGET GTest::gtest_main)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    URL_HASH SHA256=8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7
    SYSTEM
    FIND_PACKAGE_ARGS 1.14 NAMES GTest)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()
include(GoogleTest)

add_executable(inslv_tests test_learn.cpp test_equivalence.cpp)
target_link_libraries(inslv_tests PRIVATE inslv::inslv GTest::gtest_main)
target_compile_features(inslv_tests PRIVATE cxx_std_17)
target_compile_definitions(inslv_tests PRIVATE INSLV_CASES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/cases")

gtest_discover_tests(inslv_tests TEST_PREFIX "inslv." DISCOVERY_MODE PRE_TEST NO_PRETTY_VALUES
                     PROPERTIES LABELS inslv)
