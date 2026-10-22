# plantraces: las trazas de plan (formato de PDDLDomains).
planminer_add_library(plantraces trace.cpp)
target_link_libraries(plantraces PRIVATE nlohmann_json::nlohmann_json)
