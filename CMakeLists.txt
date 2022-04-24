cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_EDITOR "Build the game in the editor" ON)
option(BUILD_EMSCRIPTEN "Build with Emscripten targeting WebAssembly" OFF)

if (BUILD_EMSCRIPTEN)
    set(BUILD_EDITOR OFF)
endif ()

# Set the game to build here
set(JLE_GAME_BUILD hexablo)

set(JLE_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/engine")

add_compile_definitions(_JLE_ENGINE_PATH="${JLE_ENGINE_PATH}/")
add_compile_definitions(_GAME_RESOURCES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/${JLE_GAME_BUILD}/GameResources")

if (BUILD_EDITOR)
    add_definitions(-DBUILD_EDITOR)
endif ()

if (BUILD_EMSCRIPTEN)
    # TODO: Remove having to enable exceptions flag '-fexceptions' since not all browser engines supports exceptions?
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -o index.html -s USE_WEBGL2=1 -s USE_GLFW=3 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --shell-file ${JLE_ENGINE_PATH}/shell_fullscreen.html -s EXPORTED_RUNTIME_METHODS=cwrap,allocate -s EXPORTED_FUNCTIONS=_main,_resize_canvas_js --preload-file EditorResources --preload-file GameResources --preload-file EngineResources")
endif ()

project(${JLE_GAME_BUILD})

add_subdirectory(engine)
add_subdirectory(${JLE_GAME_BUILD})
