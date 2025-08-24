##
## Project LibFileShareProtocol, 2023
##
## Author Francois Michaut
##
## Started on  Wed Jul 10 10:27:39 2024 Francois Michaut
## Last update Sun Aug 24 20:34:12 2025 Francois Michaut
##
## CMakeLists.txt : CMake to fetch and build the dependecies of the FileShare executable
##

include(FetchContent)

FetchContent_Declare(
  argparse
  GIT_REPOSITORY  https://github.com/p-ranav/argparse.git
  GIT_TAG         af442b4da0cd7a07b56fa709bd16571889dc7fda
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
  GIT_TAG         822ed4ce6ad4229652b5b4269ff70fbf845ea1ed
)

FetchContent_MakeAvailable(fsp argparse SFML sqlite_orm tgui)
