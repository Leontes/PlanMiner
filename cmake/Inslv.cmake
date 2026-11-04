# INSLV, el aprendizaje de reglas difusas (https://github.com/Leontes/INSLV-PlanMiner), fijado en un
# commit. Sus tests se pasan en su repositorio; al incluirlo, no se compilan. Para usar una copia
# local: -DFETCHCONTENT_SOURCE_DIR_INSLV=<directorio>.

include(FetchContent)

FetchContent_Declare(
  inslv
  GIT_REPOSITORY https://github.com/Leontes/INSLV-PlanMiner.git
  GIT_TAG 7492d4a9a21e411633ba701bfe6d9863d7314e51
  SYSTEM)

block(SCOPE_FOR VARIABLES PROPAGATE inslv_SOURCE_DIR)
  # INSLV añade sus módulos detrás de los de quien lo incluye, y los dos tienen un Sanitizers.cmake.
  set(CMAKE_MODULE_PATH "")
  set(INSLV_SANITIZERS ${PLANMINER_SANITIZERS})
  # Siempre optimizado: no se depura aquí y en Debug el genético es mucho más lento.
  string(APPEND CMAKE_CXX_FLAGS_DEBUG " -O2")
  FetchContent_MakeAvailable(inslv)
endblock()
