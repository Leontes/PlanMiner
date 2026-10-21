# Avisos del compilador para el código nuevo. Old/ se compila con sus propias opciones.

function(planminer_set_warnings target)
  target_compile_options(
    ${target}
    PRIVATE -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Woverloaded-virtual
            -Wcast-align
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
            # Salir de una función no-void sin return es lo que hacía fallar el código
            # original con los compiladores actuales: siempre es un error.
            -Werror=return-type)

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${target} PRIVATE -Wduplicated-cond -Wduplicated-branches
                                             -Wlogical-op)
  endif()

  if(PLANMINER_WARNINGS_AS_ERRORS)
    target_compile_options(${target} PRIVATE -Werror)
  endif()
endfunction()
