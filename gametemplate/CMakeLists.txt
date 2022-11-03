# Copyright (c) 2022. Johan Lind

# This is a simple example of how the CMakeLists.txt can look like to set up a game project

cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

# Set the path to the engine folder here! NOTE: It must be an absolute path!
# This is one way of doing it, if the game is next to the engine folder:
set(JLE_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
cmake_path(GET JLE_ENGINE_PATH PARENT_PATH enginePath)
set(JLE_ENGINE_PATH "${enginePath}/engine")

message("The absolute engine path is \"${JLE_ENGINE_PATH}\"")

include(${JLE_ENGINE_PATH}/cmake/game1.cmake)

project(${JLE_GAME_BUILD})

add_executable(${JLE_GAME_BUILD}
        "GameTemplate.cpp"
        "main.cpp")

include(${JLE_ENGINE_PATH}/cmake/game2.cmake)