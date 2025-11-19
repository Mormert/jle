cmake_minimum_required(VERSION 3.27)
project(jlECS)

set(CMAKE_CXX_STANDARD 17)

add_library(jlECS STATIC
        src/jlECS.cpp)

target_include_directories(jlECS PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/public/jlECS")

target_include_directories(jlECS SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
