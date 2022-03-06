cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

# Set the game to build here
set(JLE_GAME_BUILD hexablo)

add_compile_definitions( _JLE_ENGINE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/engine")
add_compile_definitions( _GAME_RESOURCES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/${JLE_GAME_BUILD}/GameResources" )

project(${JLE_GAME_BUILD})

add_subdirectory(engine)
add_subdirectory(${JLE_GAME_BUILD})