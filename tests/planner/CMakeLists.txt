# Validación de dominios aprendidos: PyPlanner planifica con el dominio aprendido y VAL valida
# los planes con el de referencia (evaluate_domain.py).

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

# Línea base: el dominio que aprende Old/ con las trazas completas de BlocksWorld. El de las
# trazas incompletas no está: su STACK exige (ontable ?param_1) y deja los tres problemas sin
# solución.
planminer_add_domain_evaluation(
  legacy.blocksworld "${PROJECT_SOURCE_DIR}/tests/oracle/golden/blocksworld.pddl"
  "${blocksworld}/domain.pddl" ${blocksworld_problems})
