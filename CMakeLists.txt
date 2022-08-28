##
## Project FileShare, 2022
##
## Author Francois Michaut
##
## Started on  Sat Jan 15 01:19:13 2022 Francois Michaut
## Last update Sat Aug 27 21:20:16 2022 Francois Michaut
##
## CMakeLists.txt : Top level CMake
##

cmake_minimum_required (VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(FileShare VERSION 0.1.0 LANGUAGES C CXX)
configure_file(include/FileShareVersion.hpp.in FileShareVersion.hpp)

add_subdirectory(lib/libfsp) # Lib FileShare Protocol

file(GLOB LIB_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/lib/*/include")
file(GLOB LIB_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/lib/*")

include_directories(
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${LIB_INCLUDES}"
  "${LIB_OUTPUTS}"
  )

add_executable(file-share
  source/Sockets/Socket.cpp
  source/Sockets/Address.cpp
  source/Sockets/IPv4.cpp
  source/main.cpp
  )

target_link_libraries(file-share tgui sfml-graphics sfml-window sfml-system fsp)

add_custom_target(test
  COMMAND ${CMAKE_COMMAND}
  -B "${CMAKE_BINARY_DIR}/tests"
  -S "${CMAKE_SOURCE_DIR}/tests"
  -G ${CMAKE_GENERATOR}
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color
  --switch=$(COLOR) --cyan "Building tests..."
  COMMAND ${CMAKE_COMMAND}
  --build "${CMAKE_BINARY_DIR}/tests"
  COMMAND ${CMAKE_MAKE_PROGRAM}
  -C "${CMAKE_BINARY_DIR}/tests" test
  )
