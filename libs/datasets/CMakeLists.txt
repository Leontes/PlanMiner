# datasets: las tablas de ejemplos de cada acción y lo que se extrae de ellas (efectos numéricos,
# relaciones numéricas, discretización).
planminer_add_library(datasets discretization.cpp effects.cpp relations.cpp table.cpp)
target_link_libraries(datasets PUBLIC plantraces regression)
