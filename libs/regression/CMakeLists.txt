# regression: la regresión simbólica (el A* sobre expresiones aritméticas).
planminer_add_library(regression expression.cpp search.cpp)
target_link_libraries(regression PRIVATE resources)
