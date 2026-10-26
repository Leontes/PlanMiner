# plantraces: las trazas de plan (formato de PDDLDomains), sus acciones y la jerarquía de tipos.
planminer_add_library(plantraces actions.cpp trace.cpp types.cpp)
target_link_libraries(plantraces PRIVATE nlohmann_json::nlohmann_json)
