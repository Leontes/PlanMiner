# Old/: la implementación original, compilada como oráculo de los tests.
#
# Da dos cosas:
# - el ejecutable PlanMinerLegacy, para comparar el pipeline completo (el PDDL de salida);
# - el target planminer::legacy, para enlazar los tests con las funciones originales y
#   compararlas con las portadas.

add_subdirectory("${PROJECT_SOURCE_DIR}/Old" "${PROJECT_BINARY_DIR}/legacy")

# Las librerías de Old/, en orden de enlace (cada una antes de las que usa).
set(PLANMINER_LEGACY_LIBRARIES ruLib inslvLib dsLib ptLib stateLib taskLib)

# El oráculo se compila siempre optimizado, como se generaron las salidas de referencia: en
# Debug el genético de INSLV es muy lento.
foreach(legacy_target IN LISTS PLANMINER_LEGACY_LIBRARIES ITEMS PlanMinerLegacy)
  target_compile_options(${legacy_target} PRIVATE -O2)
endforeach()

add_library(planminer_legacy INTERFACE)
add_library(planminer::legacy ALIAS planminer_legacy)
# SYSTEM: los avisos de las cabeceras de Old/ no cuentan en los tests.
target_include_directories(planminer_legacy SYSTEM INTERFACE "${PROJECT_SOURCE_DIR}/Old/include")
target_link_libraries(planminer_legacy INTERFACE ${PLANMINER_LEGACY_LIBRARIES})
