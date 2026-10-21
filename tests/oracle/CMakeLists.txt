# Comparaciones con Old/, el oráculo mientras se porta el código.

set(PLANMINER_LEGACY_TRACES "${PROJECT_SOURCE_DIR}/Old/Test")

# Función a función: los tests enlazan con las librerías de Old/ (planminer::legacy). A medida
# que se porte cada parte, su test comparará la versión nueva con la original.
add_executable(planminer_oracle_tests test_legacy_parser.cpp)
target_link_libraries(planminer_oracle_tests PRIVATE planminer::legacy GTest::gtest_main)
target_compile_features(planminer_oracle_tests PRIVATE cxx_std_20)
target_compile_definitions(planminer_oracle_tests
                           PRIVATE PLANMINER_LEGACY_TRACES="${PLANMINER_LEGACY_TRACES}")
planminer_set_warnings(planminer_oracle_tests)

gtest_discover_tests(planminer_oracle_tests TEST_PREFIX "oracle." DISCOVERY_MODE PRE_TEST
                     NO_PRETTY_VALUES PROPERTIES LABELS oracle)

# El pipeline completo: el dominio que aprende el original con cada traza tiene que coincidir
# con el guardado en golden/. Para regenerarlos:
#   PLANMINER_UPDATE_GOLDEN=1 ctest --preset debug -R oracle.legacy
function(planminer_add_legacy_golden name trace domain_name)
  add_test(
    NAME oracle.legacy.${name}
    COMMAND
      "${CMAKE_COMMAND}" "-DLEGACY=$<TARGET_FILE:PlanMinerLegacy>"
      "-DTRACE=${PLANMINER_LEGACY_TRACES}/${trace}" "-DDOMAIN_NAME=${domain_name}"
      "-DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/golden/${name}.pddl"
      "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/legacy/${name}" -P
      "${CMAKE_CURRENT_SOURCE_DIR}/RunLegacy.cmake")
  set_tests_properties(oracle.legacy.${name} PROPERTIES LABELS oracle TIMEOUT 600)
endfunction()

planminer_add_legacy_golden(blocksworld "BlocksWorld_STRIPS.pts" blocksworld)
planminer_add_legacy_golden(blocksworld_incomplete "BlocksWorld_STRIPS(incomplete).pts"
                            blocksworld)
planminer_add_legacy_golden(depots "Depots_Numerical.pts" depots)
# Zenotravel no está: el original tarda 5 minutos y llega a 7 GB (el A* agota los 300 s en
# REFUEL).
