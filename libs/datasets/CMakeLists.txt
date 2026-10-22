# datasets: las tablas de ejemplos de cada acción y lo que se extrae de ellas (efectos numéricos).
planminer_add_library(datasets effects.cpp table.cpp)
target_link_libraries(datasets PUBLIC plantraces regression)
