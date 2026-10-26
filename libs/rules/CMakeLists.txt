# rules: el aprendizaje de reglas (INSLV) con los ejemplos de cada acción y su interpretación.
planminer_add_library(rules examples.cpp fusion.cpp legacy.cpp)
target_link_libraries(rules PUBLIC datasets inslv::inslv)
