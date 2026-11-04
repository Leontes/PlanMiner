# Comparaciones con Old/, el oráculo mientras se porta el código.

set(PLANMINER_LEGACY_TRACES "${PROJECT_SOURCE_DIR}/Old/Test")

# Función a función: los tests enlazan con las librerías de Old/ (planminer::legacy). A medida
# que se porte cada parte, su test comparará la versión nueva con la original.
add_executable(
  planminer_oracle_tests
  legacy_rules.cpp
  pts.cpp
  test_legacy_discretization.cpp
  test_legacy_parser.cpp
  test_legacy_rules.cpp
  test_legacy_tables.cpp
  test_legacy_types.cpp)
target_link_libraries(planminer_oracle_tests PRIVATE planminer::legacy datasets plantraces rules
                                                     planminer_test_main)
target_compile_features(planminer_oracle_tests PRIVATE cxx_std_20)
target_compile_definitions(
  planminer_oracle_tests
  PRIVATE PLANMINER_LEGACY_TRACES="${PLANMINER_LEGACY_TRACES}"
          PLANMINER_TRACES_DIR="${PROJECT_SOURCE_DIR}/tests/data/traces"
          PLANMINER_ORACLE_WORK_DIR="${CMAKE_CURRENT_BINARY_DIR}/work")
planminer_set_warnings(planminer_oracle_tests)

gtest_discover_tests(planminer_oracle_tests TEST_PREFIX "oracle." DISCOVERY_MODE PRE_TEST
                     NO_PRETTY_VALUES PROPERTIES LABELS oracle)

# El original reserva unos 1,6 GB en cada ejecución (dos vectores de 10^8 enteros en inslv()): se
# ejecuta con este límite y de uno en uno (RESOURCE_LOCK), para no agotar la memoria con ctest en
# paralelo.
set(PLANMINER_LEGACY_MEMORY_MB 2560 CACHE STRING "Límite de memoria (MB) de cada ejecución de Old/")

# De principio a fin: el dominio de la configuración original nueva frente al que escribe el
# original con las mismas trazas. Cada test lanza el original: de uno en uno.
add_executable(planminer_oracle_domain pts.cpp test_legacy_domain.cpp)
target_link_libraries(planminer_oracle_domain PRIVATE pddl plantraces planminer_test_main)
target_compile_features(planminer_oracle_domain PRIVATE cxx_std_20)
target_compile_definitions(
  planminer_oracle_domain
  PRIVATE PLANMINER_TRACES_DIR="${PROJECT_SOURCE_DIR}/tests/data/traces"
          PLANMINER_ORACLE_WORK_DIR="${CMAKE_CURRENT_BINARY_DIR}/work"
          PLANMINER_LEGACY="$<TARGET_FILE:PlanMinerLegacy>"
          PLANMINER_LIMIT_MEMORY="$<TARGET_FILE:planminer_limit_memory>"
          PLANMINER_LEGACY_MEMORY_MB=${PLANMINER_LEGACY_MEMORY_MB})
add_dependencies(planminer_oracle_domain PlanMinerLegacy planminer_limit_memory)
planminer_set_warnings(planminer_oracle_domain)

gtest_discover_tests(planminer_oracle_domain TEST_PREFIX "oracle." DISCOVERY_MODE PRE_TEST
                     NO_PRETTY_VALUES PROPERTIES LABELS oracle RESOURCE_LOCK legacy_memory)

# El pipeline completo: el dominio que aprende el original con cada traza tiene que coincidir
# con el guardado en golden/. Para regenerarlos:
#   PLANMINER_UPDATE_GOLDEN=1 ctest --preset debug -R oracle.legacy
function(planminer_add_legacy_golden name trace domain_name)
  add_test(
    NAME oracle.legacy.${name}
    COMMAND
      "${CMAKE_COMMAND}" "-DLEGACY=$<TARGET_FILE:PlanMinerLegacy>"
      "-DLIMIT=$<TARGET_FILE:planminer_limit_memory>" "-DMEMORY_MB=${PLANMINER_LEGACY_MEMORY_MB}"
      "-DTRACE=${PLANMINER_LEGACY_TRACES}/${trace}" "-DDOMAIN_NAME=${domain_name}"
      "-DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/golden/${name}.pddl"
      "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/legacy/${name}" -P
      "${CMAKE_CURRENT_SOURCE_DIR}/RunLegacy.cmake")
  set_tests_properties(oracle.legacy.${name} PROPERTIES LABELS oracle TIMEOUT 600 RESOURCE_LOCK
                                                         legacy_memory)
endfunction()

planminer_add_legacy_golden(blocksworld "BlocksWorld_STRIPS.pts" blocksworld)
planminer_add_legacy_golden(blocksworld_incomplete "BlocksWorld_STRIPS(incomplete).pts"
                            blocksworld)
planminer_add_legacy_golden(depots "Depots_Numerical.pts" depots)
# Zenotravel no está: el original tarda 5 minutos y llega a 7 GB (el A* agota los 300 s en
# REFUEL).

# Los casos de equivalencia de INSLV (tests/datos/planminer en su repositorio): la entrada y la
# salida de inslv() en el pipeline original. Para regenerarlos, con una copia local de INSLV
# (-DFETCHCONTENT_SOURCE_DIR_INSLV=<directorio>):
#   PLANMINER_UPDATE_GOLDEN=1 ctest --preset debug -R oracle.inslv_cases
add_executable(planminer_dump_inslv_cases dump_inslv_cases.cpp)
target_link_libraries(planminer_dump_inslv_cases PRIVATE planminer::legacy)
target_compile_features(planminer_dump_inslv_cases PRIVATE cxx_std_20)
planminer_set_warnings(planminer_dump_inslv_cases)

set(inslv_traces
    "blocksworld=${PLANMINER_LEGACY_TRACES}/BlocksWorld_STRIPS.pts"
    "blocksworld_incomplete=${PLANMINER_LEGACY_TRACES}/BlocksWorld_STRIPS(incomplete).pts"
    "depots=${PLANMINER_LEGACY_TRACES}/Depots_Numerical.pts")
add_test(
  NAME oracle.inslv_cases
  COMMAND
    "${CMAKE_COMMAND}" "-DGENERATOR=$<TARGET_FILE:planminer_dump_inslv_cases>"
    "-DLIMIT=$<TARGET_FILE:planminer_limit_memory>" "-DMEMORY_MB=${PLANMINER_LEGACY_MEMORY_MB}"
    "-DTRACES=${inslv_traces}" "-DEXPECTED_DIR=${inslv_SOURCE_DIR}/tests/datos/planminer"
    "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/inslv_cases" -P
    "${CMAKE_CURRENT_SOURCE_DIR}/RunInslvCases.cmake")
set_tests_properties(oracle.inslv_cases PROPERTIES LABELS oracle TIMEOUT 900 RESOURCE_LOCK
                                                   legacy_memory)
