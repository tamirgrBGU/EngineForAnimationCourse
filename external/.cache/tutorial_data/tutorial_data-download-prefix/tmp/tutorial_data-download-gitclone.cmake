
if(NOT "C:/Dev/EngineIGLnew/external/.cache/tutorial_data/tutorial_data-download-prefix/src/tutorial_data-download-stamp/tutorial_data-download-gitinfo.txt" IS_NEWER_THAN "C:/Dev/EngineIGLnew/external/.cache/tutorial_data/tutorial_data-download-prefix/src/tutorial_data-download-stamp/tutorial_data-download-gitclone-lastrun.txt")
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: 'C:/Dev/EngineIGLnew/external/.cache/tutorial_data/tutorial_data-download-prefix/src/tutorial_data-download-stamp/tutorial_data-download-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E remove_directory "C:/Dev/EngineIGLnew/cmake/../external/../tutorial/data"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: 'C:/Dev/EngineIGLnew/cmake/../external/../tutorial/data'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe" -c http.sslVerify=false clone --config advice.detachedHead=false "https://github.com/libigl/libigl-tutorial-data" "data"
    WORKING_DIRECTORY "C:/Dev/EngineIGLnew/cmake/../external/../tutorial"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/libigl/libigl-tutorial-data'")
endif()

execute_process(
  COMMAND "C:/Program Files/Git/cmd/git.exe" -c http.sslVerify=false checkout 5c6a1ea809c043d71e5595775709c15325a7158c --
  WORKING_DIRECTORY "C:/Dev/EngineIGLnew/cmake/../external/../tutorial/data"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: '5c6a1ea809c043d71e5595775709c15325a7158c'")
endif()

execute_process(
  COMMAND "C:/Program Files/Git/cmd/git.exe" -c http.sslVerify=false submodule update --recursive --init 
  WORKING_DIRECTORY "C:/Dev/EngineIGLnew/cmake/../external/../tutorial/data"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: 'C:/Dev/EngineIGLnew/cmake/../external/../tutorial/data'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "C:/Dev/EngineIGLnew/external/.cache/tutorial_data/tutorial_data-download-prefix/src/tutorial_data-download-stamp/tutorial_data-download-gitinfo.txt"
    "C:/Dev/EngineIGLnew/external/.cache/tutorial_data/tutorial_data-download-prefix/src/tutorial_data-download-stamp/tutorial_data-download-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: 'C:/Dev/EngineIGLnew/external/.cache/tutorial_data/tutorial_data-download-prefix/src/tutorial_data-download-stamp/tutorial_data-download-gitclone-lastrun.txt'")
endif()

