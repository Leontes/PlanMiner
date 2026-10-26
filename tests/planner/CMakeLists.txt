# Los tests con PyPlanner, VAL y PDDLDomains: el arnés que valida dominios aprendidos
# (scripts/evaluate_domain.py), las trazas de prueba de tests/data/traces, que tienen que
# regenerarse igual (scripts/generate_traces.py), y el banco de pruebas mínimo
# (benchmarks/smoke.toml, scripts/benchmark.py).

set(blocksworld "${PROJECT_SOURCE_DIR}/tests/data/blocksworld")
set(blocksworld_problems "${blocksworld}/p01.pddl" "${blocksworld}/p02.pddl"
                         "${blocksworld}/p03.pddl")

function(planminer_add_domain_evaluation name learned reference)
  add_test(
    NAME planner.${name}
    COMMAND "${PLANMINER_PYTHON}" "${PROJECT_SOURCE_DIR}/scripts/evaluate_domain.py" --learned
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

# El banco de pruebas mínimo, de principio a fin: genera los datos, que tienen que coincidir con
# las huellas de benchmarks/smoke.lock.json; hace la validación cruzada con el PlanMiner recién
# compilado (--force: aunque haya resultados de antes); y escribe el informe. Un solo trabajo:
# cabe junto a los demás tests en paralelo.
set(benchmark "${PLANMINER_PYTHON}" "${PROJECT_SOURCE_DIR}/scripts/benchmark.py")
set(smoke --output "${CMAKE_CURRENT_BINARY_DIR}/benchmark-smoke")
add_test(NAME planner.benchmark.generate
         COMMAND ${benchmark} generate smoke ${smoke} --validate "${PLANMINER_VAL}" --jobs 1)
add_test(NAME planner.benchmark.run
         COMMAND ${benchmark} run smoke ${smoke} --validate "${PLANMINER_VAL}" --jobs 1 --force
                 --planminer $<TARGET_FILE:planminer>)
add_test(NAME planner.benchmark.report COMMAND ${benchmark} report smoke ${smoke})
set_tests_properties(planner.benchmark.generate planner.benchmark.run planner.benchmark.report
                     PROPERTIES LABELS planner TIMEOUT 900)
set_tests_properties(planner.benchmark.generate PROPERTIES FIXTURES_SETUP benchmark_smoke_data)
set_tests_properties(planner.benchmark.run PROPERTIES FIXTURES_REQUIRED benchmark_smoke_data
                                                      FIXTURES_SETUP benchmark_smoke_runs)
set_tests_properties(planner.benchmark.report PROPERTIES FIXTURES_REQUIRED benchmark_smoke_runs)
