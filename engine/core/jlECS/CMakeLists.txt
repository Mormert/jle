cmake_minimum_required(VERSION 3.27)
project(jlecs)

set(CMAKE_CXX_STANDARD 17)

add_library(jlECS STATIC
        jlECS.cpp
        jlECSSaveLoad.cpp)
