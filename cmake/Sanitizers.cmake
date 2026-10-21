# AddressSanitizer y UndefinedBehaviorSanitizer para el código nuevo (PLANMINER_SANITIZERS).
#
# Las opciones de enlace son públicas: un ejecutable que enlace con una librería
# instrumentada necesita el runtime de los sanitizers.

function(planminer_enable_sanitizers target)
  if(NOT PLANMINER_SANITIZERS)
    return()
  endif()
  target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer
                                           -fno-sanitize-recover=undefined)
  target_link_options(${target} PUBLIC -fsanitize=address,undefined)
endfunction()
