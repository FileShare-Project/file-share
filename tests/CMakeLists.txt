##
## Project FileShare-Tests, 2022
##
## Author Francois Michaut
##
## Started on  Mon Feb 14 19:35:41 2022 Francois Michaut
## Last update Wed Feb 16 23:17:37 2022 Francois Michaut
##
## CMakeLists.txt : CMake building and running tests for FileShare
##

cmake_minimum_required (VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(FileShare-Tests VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)

create_test_sourcelist (TestFiles test_driver.cpp
  Sockets/TestSockets.cpp
  )

add_executable(unit_tests
  ../source/Sockets/Address.cpp
  ../source/Sockets/Socket.cpp
  ../source/Sockets/IPv4.cpp
  ${TestFiles}
  )

foreach (test ${TestFiles})
  if (NOT ${test} STREQUAL test_driver.cpp)
    get_filename_component (DName ${test} DIRECTORY)
    get_filename_component (TName ${test} NAME_WE)
    add_test (NAME ${DName}/${TName} COMMAND unit_tests ${DName}/${TName})
  endif()
endforeach ()

include_directories(
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)
