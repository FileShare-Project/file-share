##
## Project FileShare, 2022
##
## Author Francois Michaut
##
## Started on  Sat Jan 15 01:19:13 2022 Francois Michaut
## Last update Wed Jul 19 08:59:15 2023 Francois Michaut
##
## CMakeLists.txt : Top level CMake
##

cmake_minimum_required (VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# TODO: This currently breaks Apple/Windows builds: find another solution
set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS 1)
set(MAKEFLAGS "--no-print-directory")

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -DDEBUG -g3")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3")
add_compile_definitions(_GNU_SOURCE)

project(FileShare VERSION 0.1.0 LANGUAGES C CXX)
configure_file(include/FileShareVersion.hpp.in FileShareVersion.hpp)

### BEGIN RECURSIVE LIBRARY INCLUSION ###

# TODO run 'git submodule update --init --recursive'

function(discover_sub_libraries LIB_INCLUDES LIB_SOURCES)
  # Set library_dir to optional argument or default value
  set(LIBRARY_DIR "lib")
  list(LENGTH ARGN ARGNC)
  if (${ARGNC} GREATER 0)
    list(GET ARGN 0 LIBRARY_DIR)
  endif()

  set(RECURSIVE_LIST ${LIBRARY_DIR})
  while("${RECURSIVE_LIST}" STRGREATER "")
    # ITEM is the path relative to the current directory
    # CURRENT_PATH is the same directory but in absolute form
    list(POP_FRONT RECURSIVE_LIST ITEM)
    cmake_path(SET CURRENT_PATH "${ITEM}")
    cmake_path(ABSOLUTE_PATH CURRENT_PATH)
    file(GLOB CHILDS RELATIVE "${CURRENT_PATH}" "${CURRENT_PATH}/*")
    foreach(CHILD ${CHILDS})
      # TODO add checks for presence of include/CMakeLists.txt ?
      if(IS_DIRECTORY ${CURRENT_PATH}/${CHILD})
        list(APPEND LIBRARY_SOURCES "${CURRENT_PATH}/${CHILD}")
        list(APPEND LIBRARY_INCLUDES
          "${CURRENT_PATH}/${CHILD}/include"
          "${CMAKE_CURRENT_BINARY_DIR}/${ITEM}/${CHILD}")
        list(APPEND RECURSIVE_LIST "${ITEM}/${CHILD}/${LIBRARY_DIR}")
      endif()
    endforeach()
  endwhile()
  # Return to destination variables
  set(${LIB_INCLUDES} "${LIBRARY_INCLUDES}" PARENT_SCOPE)
  set(${LIB_SOURCES} "${LIBRARY_SOURCES}" PARENT_SCOPE)
endfunction()

### END RECURSIVE LIBRARY INCLUSION ###

discover_sub_libraries(LIB_INCLUDES LIB_SOURCES)

add_subdirectory(lib/libfsp) # Lib FileShare Protocol

include_directories(
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${LIB_INCLUDES}"
)

add_executable(file-share
  source/main.cpp
)

target_link_libraries(file-share tgui sfml-graphics sfml-window sfml-system fsp)

add_custom_target(test
  COMMAND ${CMAKE_COMMAND} --log-level=WARNING
  -B "${CMAKE_CURRENT_BINARY_DIR}/tests"
  -S "${CMAKE_CURRENT_SOURCE_DIR}/tests"
  -G ${CMAKE_GENERATOR}
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color
  --switch=$(COLOR) --cyan "Building tests..."
  COMMAND ${CMAKE_COMMAND}
  --build "${CMAKE_CURRENT_BINARY_DIR}/tests" -- --quiet
  COMMAND ${CMAKE_MAKE_PROGRAM}
  -C "${CMAKE_CURRENT_BINARY_DIR}/tests" test ARGS=--output-on-failure
  )
