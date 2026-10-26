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

# El dominio que aprende la línea de comandos (por ahora, con la configuración del original) con
# las trazas completas de blocksworld, evaluado con problemas del catálogo que no están en las
# trazas.
set(blocks "${PROJECT_SOURCE_DIR}/vendor/PDDLDomains/dominios/ipc-2000/blocks-strips-untyped")
file(GLOB blocks_traces
     "${PROJECT_SOURCE_DIR}/tests/data/traces/blocks-strips-untyped/*.complete.json")
add_test(NAME planner.learn.blocksworld
         COMMAND planminer -o "${CMAKE_CURRENT_BINARY_DIR}/blocks.pddl" ${blocks_traces})
set_tests_properties(planner.learn.blocksworld PROPERTIES LABELS planner FIXTURES_SETUP
                                                          learned_blocksworld)
planminer_add_domain_evaluation(
  learned.blocksworld "${CMAKE_CURRENT_BINARY_DIR}/blocks.pddl" "${blocks}/domain.pddl"
  "${blocks}/p09.pddl" "${blocks}/p10.pddl" "${blocks}/p11.pddl")
set_tests_properties(planner.learned.blocksworld PROPERTIES FIXTURES_REQUIRED learned_blocksworld)
