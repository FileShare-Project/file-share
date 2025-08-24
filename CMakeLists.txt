##
## Project FileShare, 2022
##
## Author Francois Michaut
##
## Started on  Sat Jan 15 01:19:13 2022 Francois Michaut
## Last update Sun Aug 24 18:59:20 2025 Francois Michaut
##
## CMakeLists.txt : Top level CMake
##

cmake_minimum_required (VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

set(MAKEFLAGS "--no-print-directory")

option(BUILD_CLI "Build the FileShare CLI" ON)
option(BUILD_GUI "Build the FileShare GUI" ON)

if (!WIN32)
  set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -DDEBUG -g3")
  set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3")
  add_compile_definitions(_GNU_SOURCE _FILE_OFFSET_BITS=64 _TIME_BITS=64)
endif()

project(FileShare VERSION 0.1.0 LANGUAGES C CXX)
configure_file(include/FileShareVersion.hpp.in FileShareVersion.hpp)

add_subdirectory(dependencies)

include_directories(
  "${CMAKE_CURRENT_BINARY_DIR}" # For the configured Version file
  "${CMAKE_CURRENT_SOURCE_DIR}/include" # For the regular includes
)

set(mvc_sources "")
function(add_mvc_sources)
  set(local_sources ${mvc_sources})

  foreach(name IN LISTS ARGN)
    set(new_sources
      "source/${name}/View.cpp"
      "source/${name}/Model.cpp"
      "source/${name}/Controller.cpp"
    )

    set(local_sources ${local_sources} ${new_sources})
  endforeach()

  set(mvc_sources ${local_sources} PARENT_SCOPE)
endfunction()

if (BUILD_GUI)
  add_mvc_sources(DeviceList Settings)
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
    ${mvc_sources}
  )

  target_compile_definitions(file-share PRIVATE _LARGEFILE_SOURCE _FILE_OFFSET_BITS=64)
  target_link_libraries(file-share tgui sfml-graphics sfml-window sfml-system sqlite_orm::sqlite_orm fsp)

  # Platform-specific libraries for theme detection
  if(APPLE)
    target_link_libraries(file-share "-framework CoreFoundation")
  elseif(WIN32)
    target_link_libraries(file-share winreg)
  endif()

  target_compile_options(file-share PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
  )
endif()

if (BUILD_CLI)
  add_executable(file-share-cli
    source/cli/main.cpp
    source/cli/interactive.cpp
  )

  target_compile_definitions(file-share-cli PRIVATE _LARGEFILE_SOURCE _FILE_OFFSET_BITS=64)
  target_link_libraries(file-share-cli argparse fsp)
  if (!APPLE)
    target_precompile_headers(file-share-cli
      PRIVATE "${argparse_SOURCE_DIR}/include/argparse/argparse.hpp"
    )
  endif()

  target_compile_options(file-share-cli PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
  )
endif()

option(FILE_SHARE_BUILD_TESTS "TRUE to build the FileShare tests" FALSE)
if(FILE_SHARE_BUILD_TESTS)
  add_subdirectory(tests)
endif()
