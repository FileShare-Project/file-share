##
## Project FileShare, 2022
##
## Author Francois Michaut
##
## Started on  Sat Jan 15 01:19:13 2022 Francois Michaut
## Last update Fri Nov 24 10:00:00 2023 Francois Michaut
##
## CMakeLists.txt : Top level CMake
##

cmake_minimum_required (VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(ExternalProject)
include(FetchContent)

# TODO: This currently breaks Apple/Windows builds: find another solution
# set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS 1) 
set(MAKEFLAGS "--no-print-directory")

if (!WIN32)
  set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -DDEBUG -g3")
  set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3")
  add_compile_definitions(_GNU_SOURCE)
endif()

project(FileShare VERSION 0.1.0 LANGUAGES C CXX)
configure_file(include/FileShareVersion.hpp.in FileShareVersion.hpp)

add_subdirectory(dependencies)

include_directories(
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set(executables "")

function(add_mvc_source)
  set(local_executables ${executables})

  foreach(name IN LISTS ARGN)
    set(new_executables
      "source/${name}/View.cpp"
      "source/${name}/Model.cpp"
      "source/${name}/Controller.cpp"
    )

    set(local_executables ${local_executables} ${new_executables})
  endforeach()

  set(executables ${local_executables} PARENT_SCOPE)
endfunction()

add_mvc_source(DeviceList Settings)
add_executable(file-share
  source/main.cpp
  source/Application.cpp
  source/Debug.cpp
  source/ThemeManager.cpp
  source/Components/Button.cpp
  source/Components/Foldout.cpp
  source/Components/InputFileDialog.cpp
  source/Components/List.cpp
  source/Components/ListMenu.cpp
  ${executables}
)

target_compile_definitions(file-share PRIVATE _LARGEFILE_SOURCE _FILE_OFFSET_BITS=64)
target_link_libraries(file-share tgui sfml-graphics sfml-window sfml-system sqlite_orm::sqlite_orm fsp)
target_compile_options(file-share PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_executable(file-share-cli
  source/cli/main.cpp
)

target_compile_definitions(file-share-cli PRIVATE _LARGEFILE_SOURCE _FILE_OFFSET_BITS=64)
target_link_libraries(file-share-cli argparse fsp)
if (!APPLE)
  target_precompile_headers(file-share-cli
    PRIVATE
    "${argparse_SOURCE_DIR}/include/argparse/argparse.hpp"
  )
endif()
target_compile_options(file-share-cli PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

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
