# nlohmann/json, para leer las trazas: se usa el instalado en el sistema si es 3.11 o posterior
# (nlohmann-json3-dev); si no, se descarga la 3.11.3. Para forzar la descarga:
# -DFETCHCONTENT_TRY_FIND_PACKAGE_MODE=NEVER.

include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
  SYSTEM
  FIND_PACKAGE_ARGS 3.11 NAMES nlohmann_json)

FetchContent_MakeAvailable(nlohmann_json)
