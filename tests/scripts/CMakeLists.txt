# Los tests de los scripts de Python que no necesitan PyPlanner ni VAL (compare_domains.py).
find_package(Python3 3.12 COMPONENTS Interpreter)
if(NOT Python3_Interpreter_FOUND)
  message(STATUS "Tests de los scripts desactivados: falta Python 3.12 o posterior")
  return()
endif()

add_test(NAME unit.scripts.compare_domains
         COMMAND Python3::Interpreter -m unittest discover -s "${CMAKE_CURRENT_SOURCE_DIR}" -p
                 "test_compare_domains.py")
set_tests_properties(unit.scripts.compare_domains
                     PROPERTIES LABELS unit ENVIRONMENT "PYTHONPATH=${PROJECT_SOURCE_DIR}/scripts")
