# Dependencias de los tests.
#
# GoogleTest: se usa el instalado en el sistema si es 1.14 o posterior (libgtest-dev);
# si no, se descarga la 1.14.0. Para forzar la descarga:
# -DFETCHCONTENT_TRY_FIND_PACKAGE_MODE=NEVER.

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
