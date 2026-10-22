# plantraces: las trazas de plan (formato de PDDLDomains) y sus acciones.
planminer_add_library(plantraces actions.cpp trace.cpp)
target_link_libraries(plantraces PRIVATE nlohmann_json::nlohmann_json)
