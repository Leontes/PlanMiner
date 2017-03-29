if("master" STREQUAL "")
  message(FATAL_ERROR "Tag for git checkout should not be empty.")
endif()

set(run 0)

if("/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest-stamp/gtest-gitinfo.txt" IS_NEWER_THAN "/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest-stamp/gtest-gitclone-lastrun.txt")
  set(run 1)
endif()

if(NOT run)
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: '/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest-stamp/gtest-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E remove_directory "/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest'")
endif()

# try the clone 3 times incase there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git" clone --origin "origin" "https://github.com/google/googletest" "gtest"
    WORKING_DIRECTORY "/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/google/googletest'")
endif()

execute_process(
  COMMAND "/usr/bin/git" checkout master
  WORKING_DIRECTORY "/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'master'")
endif()

execute_process(
  COMMAND "/usr/bin/git" submodule init 
  WORKING_DIRECTORY "/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to init submodules in: '/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest'")
endif()

execute_process(
  COMMAND "/usr/bin/git" submodule update --recursive 
  WORKING_DIRECTORY "/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest-stamp/gtest-gitinfo.txt"
    "/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest-stamp/gtest-gitclone-lastrun.txt"
  WORKING_DIRECTORY "/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/mnt/Datos/Dropbox/Doctorado/Investigación/PlanMiner_Abril17/PlanMiner/gtest/src/gtest-stamp/gtest-gitclone-lastrun.txt'")
endif()

