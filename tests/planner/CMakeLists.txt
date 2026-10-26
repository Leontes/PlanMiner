# Validación de dominios aprendidos: PyPlanner planifica con el dominio aprendido y VAL valida
# los planes con el de referencia (evaluate_domain.py). Y las trazas de prueba, que tienen que
# regenerarse igual (scripts/generate_traces.py).

set(blocksworld "${PROJECT_SOURCE_DIR}/tests/data/blocksworld")
set(blocksworld_problems "${blocksworld}/p01.pddl" "${blocksworld}/p02.pddl"
                         "${blocksworld}/p03.pddl")

function(planminer_add_domain_evaluation name learned reference)
  add_test(
    NAME planner.${name}
    COMMAND "${PLANMINER_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/evaluate_domain.py" --learned
            "${learned}" --reference "${reference}" --validate "${PLANMINER_VAL}" ${ARGN})
  set_tests_properties(planner.${name} PROPERTIES LABELS planner TIMEOUT 900)
endfunction()

# Comprueba el propio arnés: el dominio de referencia, evaluado contra sí mismo, da planes
# válidos...
planminer_add_domain_evaluation(reference.blocksworld "${blocksworld}/domain.pddl"
                                "${blocksworld}/domain.pddl" ${blocksworld_problems})

# ...y uno defectuoso, planes que VAL rechaza. No basta con que el arnés falle: tiene que ser
# porque VAL rechaza el plan (VAL termina con 0 en algunos planes inválidos).
planminer_add_domain_evaluation(harness.detects_invalid_plans
                                "${blocksworld}/stack-without-clear.pddl"
                                "${blocksworld}/domain.pddl" "${blocksworld}/p01.pddl")
set_tests_properties(planner.harness.detects_invalid_plans
                     PROPERTIES PASS_REGULAR_EXPRESSION "inválido +p01.pddl: Plan failed")

# Las trazas de tests/data/traces se regeneran igual a partir de sus planes.
add_test(NAME planner.traces_reproducible
         COMMAND "${PLANMINER_PYTHON}" "${PROJECT_SOURCE_DIR}/scripts/generate_traces.py" --check)
set_tests_properties(planner.traces_reproducible PROPERTIES LABELS planner TIMEOUT 900)

# Línea base: el dominio que aprende Old/ con las trazas completas de BlocksWorld. El de las
# trazas incompletas no está: su STACK exige (ontable ?param_1) y deja los tres problemas sin
# solución.
planminer_add_domain_evaluation(
  legacy.blocksworld "${PROJECT_SOURCE_DIR}/tests/oracle/golden/blocksworld.pddl"
  "${blocksworld}/domain.pddl" ${blocksworld_problems})

# El dominio que aprende la línea de comandos, con la configuración mejorada y con la original, con
# las trazas completas de blocksworld, evaluado con problemas del catálogo que no están en las
# trazas.
set(blocks "${PROJECT_SOURCE_DIR}/vendor/PDDLDomains/dominios/ipc-2000/blocks-strips-untyped")
file(GLOB blocks_traces
     "${PROJECT_SOURCE_DIR}/tests/data/traces/blocks-strips-untyped/*.complete.json")
foreach(configuration IN ITEMS improved original)
  set(learned "${CMAKE_CURRENT_BINARY_DIR}/blocks.${configuration}.pddl")
  set(flags "")
  if(configuration STREQUAL "original")
    set(flags --original)
  endif()
  add_test(NAME planner.learn.blocksworld.${configuration}
           COMMAND planminer ${flags} -o "${learned}" ${blocks_traces})
  set_tests_properties(planner.learn.blocksworld.${configuration}
                       PROPERTIES LABELS planner FIXTURES_SETUP learned_blocksworld_${configuration})
  planminer_add_domain_evaluation(
    learned.blocksworld.${configuration} "${learned}" "${blocks}/domain.pddl" "${blocks}/p09.pddl"
    "${blocks}/p10.pddl" "${blocks}/p11.pddl")
  set_tests_properties(planner.learned.blocksworld.${configuration}
                       PROPERTIES FIXTURES_REQUIRED learned_blocksworld_${configuration})
endforeach()

# Con la configuración mejorada, los dominios numéricos: zenotravel y counters aprendidos con sus
# trazas completas, con problemas que el dominio de referencia resuelve en el límite de tiempo.
foreach(spec IN ITEMS "zenotravel|p01 p02 p03 p04 p05 p06 p07 p09"
                      "counters|p01 p02 p03 p04 p05")
  string(REPLACE "|" ";" spec "${spec}")
  list(GET spec 0 name)
  list(GET spec 1 problems)
  separate_arguments(problems)
  set(catalog "${PROJECT_SOURCE_DIR}/vendor/PDDLDomains/dominios/ipc-2023-numeric/${name}")
  set(learned "${CMAKE_CURRENT_BINARY_DIR}/${name}.improved.pddl")
  file(GLOB traces "${PROJECT_SOURCE_DIR}/tests/data/traces/${name}/*.complete.json")
  add_test(NAME planner.learn.${name}.improved COMMAND planminer -o "${learned}" ${traces})
  set_tests_properties(planner.learn.${name}.improved
                       PROPERTIES LABELS planner FIXTURES_SETUP learned_${name}_improved)
  list(TRANSFORM problems PREPEND "${catalog}/")
  list(TRANSFORM problems APPEND ".pddl")
  planminer_add_domain_evaluation(learned.${name}.improved "${learned}" "${catalog}/domain.pddl"
                                  ${problems})
  set_tests_properties(planner.learned.${name}.improved
                       PROPERTIES FIXTURES_REQUIRED learned_${name}_improved)
endforeach()
