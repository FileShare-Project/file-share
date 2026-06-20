##
## Project LibFileShareProtocol, 2023
##
## Author Francois Michaut
##
## Started on  Wed Jul 10 10:27:39 2024 Francois Michaut
## Last update Sat Jun 20 10:49:33 2026 Francois Michaut
##
## CMakeLists.txt : CMake to fetch and build the dependecies of the FileShare executable
##

include(FetchContent)

FetchContent_Declare(
  argparse
  GIT_REPOSITORY  https://github.com/p-ranav/argparse.git
  GIT_TAG         3eda91b2e1ce7d569f84ba295507c4cd8fd96910
)
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML
  GIT_TAG 69ea0cd863aed1d4092b970b676924a716ff718b
  FIND_PACKAGE_ARGS COMPONENTS graphics window system
)
FetchContent_Declare(
  sqlite_orm
  GIT_REPOSITORY https://github.com/fnc12/sqlite_orm
  GIT_TAG v1.9.1
)

set(TGUI_BACKEND "SFML_GRAPHICS")
set(TGUI_BUILD_FRAMEWORK FALSE)
FetchContent_Declare(
  tgui
  GIT_REPOSITORY  https://github.com/texus/TGUI
  GIT_TAG         v1.10.0
  FIND_PACKAGE_ARGS NAMES TGUI
)

FetchContent_Declare(
  fsp
  GIT_REPOSITORY  https://github.com/FileShare-Project/libfsp.git
  GIT_TAG         5c02001451f5d611a5ba205fc6252e0ae1f2f9a6
)

FetchContent_MakeAvailable(fsp argparse SFML sqlite_orm tgui)
