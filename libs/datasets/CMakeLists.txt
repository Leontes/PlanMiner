# datasets: las tablas de ejemplos de cada acción.
planminer_add_library(datasets table.cpp)
target_link_libraries(datasets PUBLIC plantraces)
