# Los tests de los scripts de Python que no necesitan PyPlanner, VAL ni PDDLDomains
# (compare_domains.py y lo que se puede probar sin ellos de benchmark.py).
find_package(Python3 3.12 COMPONENTS Interpreter)
if(NOT Python3_Interpreter_FOUND)
  message(STATUS "Tests de los scripts desactivados: falta Python 3.12 o posterior")
  return()
endif()

add_test(NAME unit.scripts
         COMMAND Python3::Interpreter -m unittest discover -s "${CMAKE_CURRENT_SOURCE_DIR}" -p
                 "test_*.py")
set_tests_properties(unit.scripts
                     PROPERTIES LABELS unit ENVIRONMENT "PYTHONPATH=${PROJECT_SOURCE_DIR}/scripts")
