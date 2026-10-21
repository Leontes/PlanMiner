# Tests, por etiquetas de CTest (ctest -L <etiqueta>):
#
# - unit: tests unitarios del código nuevo.
# - oracle: comparaciones con Old/, función a función y con el pipeline completo.
# - planner: los dominios aprendidos se validan planificando con PyPlanner y VAL.
# - inslv: los de vendor/inslv (se registran allí).

include(GoogleTest)

add_subdirectory(unit)

if(PLANMINER_BUILD_LEGACY)
  include(Legacy)
  add_subdirectory(oracle)
else()
  message(STATUS "Tests del oráculo desactivados (PLANMINER_BUILD_LEGACY=OFF)")
endif()

include(Planner)
if(PLANMINER_PLANNER_TESTS)
  add_subdirectory(planner)
endif()
