# This cmake file can be included before the game2.cmake file
# See the template project's CMakeLists.txt.

# Sets the game build name to the same as the game directory
get_filename_component(JLE_GAME_BUILD ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${JLE_GAME_BUILD})
message("The game build name is \"${JLE_GAME_BUILD}\"")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${JLE_GAME_BUILD})