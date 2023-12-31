#############################################################################################
#                                                                                           #
#               ,     .     ,                      .   ,--.                                 #
#               |     |     |                      |   |            o                       #
#               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               #
#               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               #
#              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               #
#                                                                                           #
#     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     #
#          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       #
#     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     #
#                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 #
#                                                                                           #
#############################################################################################

# This cmake file is intended to be included in the game CMakeLists.txt
# See the template project's CMakeLists.txt.

target_include_directories(${JLE_GAME_BUILD} SYSTEM PUBLIC)

option(JLE_BUILD_EDITOR "Build the game in the editor" ON)
option(JLE_BUILD_HEADLESS "Build the game without graphics (for servers)" OFF)
option(JLE_BUILD_EMSCRIPTEN "Build with Emscripten targeting WebAssembly" OFF)
option(JLE_BUILD_OPENGLES30 "Use OpenGL ES 3.0 instead of desktop core 3.3" ON)
option(JLE_BUILD_UNITY "Smash compilation units into chunks for faster build times" OFF)
option(JLE_BUILD_PRECOMPILED_HEADER "Use the precompiled header technique for faster iteration build times" OFF)

# Mac OS doesn't support GLES 3.0
if (APPLE)
    set(JLE_BUILD_OPENGLES30 OFF)
endif ()

if (JLE_BUILD_EMSCRIPTEN)
    set(JLE_BUILD_EDITOR OFF)
    set(JLE_BUILD_OPENGLES30 ON)
endif ()

add_compile_definitions(JLE_ENGINE_PATH_CMAKE=${JLE_ENGINE_PATH}/)
add_compile_definitions(GAME_RESOURCES_DIRECTORY_CMAKE=${CMAKE_CURRENT_SOURCE_DIR}/GameResources)

if (MSVC)
    # /bigobj allows us to compile with a heavy amount of templated code
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif ()

if (MINGW)
    # This -03 issue is similar to the /bigobj fix above, but it uses more optimization...? Hacky. :>
    # The dangling-reference warning is coming from Assimp, and only happens on MinGW
    # Socket.io library uses WinSock
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wno-dangling-reference -lwsock32 -lws2_32")

    find_library(WSOCK32_LIBRARY wsock32)
    find_library(WS2_32_LIBRARY ws2_32)
    link_libraries(wsock32 ws2_32)
endif ()

if (JLE_BUILD_EDITOR)
    add_definitions(-DJLE_BUILD_EDITOR)
endif ()

if (JLE_BUILD_OPENGLES30)
    add_definitions(-DJLE_BUILD_OPENGLES30)
endif ()

if (JLE_BUILD_EMSCRIPTEN)
    # TODO: Remove having to enable exceptions flag '-fexceptions' since not all browser engines supports exceptions?
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -o index.html -s USE_WEBGL2=1 -s USE_GLFW=3 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --shell-file ${JLE_ENGINE_PATH}/shell_fullscreen.html -s EXPORTED_RUNTIME_METHODS=cwrap,allocate -s EXPORTED_FUNCTIONS=_main,_resize_canvas_js --preload-file EditorResources --preload-file GameResources --preload-file EngineResources")
endif ()


add_subdirectory("${JLE_ENGINE_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/${JLE_GAME_BUILD}")
target_link_libraries(${JLE_GAME_BUILD} LINK_PUBLIC engine)

if (JLE_BUILD_EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set_target_properties(${JLE_GAME_BUILD} PROPERTIES OUTPUT_NAME "index")
endif ()


if (JLE_BUILD_EMSCRIPTEN)
    # When building with Emscripten, it needs to find the pre-loaded assets
    # at the same location as the 'binary'
    FILE(COPY GameResources DESTINATION ${PROJECT_BINARY_DIR})
else ()
    FILE(COPY GameResources DESTINATION ${JLE_GAME_BUILD})
endif ()