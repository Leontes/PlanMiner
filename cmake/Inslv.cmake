# INSLV, el aprendizaje de reglas difusas (https://github.com/Leontes/INSLV-PlanMiner), fijado en una
# versión. Sus tests se pasan en su repositorio; al incluirlo, no se compilan. Para usar una copia
# local: -DFETCHCONTENT_SOURCE_DIR_INSLV=<directorio>.

include(FetchContent)

FetchContent_Declare(
  inslv
  GIT_REPOSITORY https://github.com/Leontes/INSLV-PlanMiner.git
  GIT_TAG 3ec2c52ae286b368f4a52d35eee3c65cd209fb8d # v1.0.0
  SYSTEM)

block(SCOPE_FOR VARIABLES PROPAGATE inslv_SOURCE_DIR)
  set(INSLV_SANITIZERS ${PLANMINER_SANITIZERS})
  # Siempre optimizado: no se depura aquí y en Debug el genético es mucho más lento.
  string(APPEND CMAKE_CXX_FLAGS_DEBUG " -O2")
  FetchContent_MakeAvailable(inslv)
endblock()
