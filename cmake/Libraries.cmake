# Las librerías de PlanMiner: una por cada parte del algoritmo, en libs/<nombre>.
#
# Cada librería tiene sus fuentes en libs/<nombre> y sus cabeceras públicas en
# libs/<nombre>/include/<nombre>, que se incluyen como <nombre>/<cabecera>.hpp. Una librería solo
# ve las cabeceras de las que enlaza: las dependencias entre partes quedan explícitas.

# planminer_add_library(<nombre> <fuentes>...): la librería estática, con C++20, los avisos y
# los sanitizers del código nuevo.
function(planminer_add_library name)
  add_library(${name} STATIC ${ARGN})
  target_include_directories(${name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
  target_compile_features(${name} PUBLIC cxx_std_20)
  planminer_set_warnings(${name})
  planminer_enable_sanitizers(${name})
endfunction()
