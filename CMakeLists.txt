
# -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

project(PONG)

add_subdirectory(engine)
add_subdirectory(Pong2)