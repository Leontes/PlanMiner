# Tests, por etiquetas de CTest (ctest -L <etiqueta>):
#
# - unit: tests unitarios del código nuevo.
# - oracle: comparaciones con Old/, función a función y con el pipeline completo.
# - planner: los dominios aprendidos se validan planificando con PyPlanner y VAL.
# - inslv: los de vendor/inslv (se registran allí).

include(GoogleTest)

# Cada proceso de los tests tiene un límite duro de memoria: en WSL, agotar la memoria congela
# todo (ver resources/memory.hpp). Los de GoogleTest lo fijan en su main (planminer_test_main,
# PLANMINER_TEST_MEMORY_MB); los programas que no son de PlanMiner se lanzan con
# planminer_limit_memory.
add_subdirectory(support)

add_subdirectory(unit)
add_subdirectory(scripts)

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
