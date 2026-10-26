# pddl: el dominio que aprende PlanMiner de principio a fin y su escritura en PDDL.
planminer_add_library(pddl domain.cpp)
target_link_libraries(pddl PUBLIC rules plantraces datasets)
