cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

project(PONG)

set(JLE_GAME_BUILD Pong2)

add_subdirectory(engine)
add_subdirectory(Pong2)