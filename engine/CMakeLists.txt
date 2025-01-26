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

option(JLE_ALLOW_MINGW "Allow using MinGW on Windows" OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()


if (WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT JLE_ALLOW_MINGW)
        message(FATAL_ERROR "MinGW is not supported on Windows by default. Please use MSVC or enable ALLOW_MINGW to proceed with MinGW.")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND JLE_ALLOW_MINGW)
        message(WARNING "Using MinGW on Windows. This is not officially supported; use at your own risk.")
    endif()
endif()

option(ENABLE_CLANG_TIMETRACE "Enable -ftime-trace for Clang" OFF)
if (ENABLE_CLANG_TIMETRACE AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-ftime-trace ${CMAKE_CXX_FLAGS}")
endif ()


add_library(engine STATIC
        "jleGameEngine.cpp"
        "modules/jleEngineUpdateContext.cpp"
        "modules/jleEditorUpdateContext.cpp"
        "modules/jleGameModules.cpp"
        "game/jleGameRuntime.cpp"
        "game/jleGame.cpp"
        "game/jleDefaultGameFunctions.cpp"
        )


target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# TODO : remove this
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


add_subdirectory(core)
target_link_libraries(engine PUBLIC jleCore)

add_subdirectory(modules)

if (JLE_BUILD_EDITOR)
    add_subdirectory(editor)

    target_link_libraries(jleEditor PUBLIC engine)
endif ()


if (JLE_BUILD_EMSCRIPTEN)
    # When building with Emscripten, it needs to find the pre-loaded assets
    # at the same location as the 'binary'
    FILE(COPY EditorResources DESTINATION ${PROJECT_BINARY_DIR})
    FILE(COPY EngineResources DESTINATION ${PROJECT_BINARY_DIR})
else ()
    FILE(COPY EditorResources DESTINATION ${PROJECT_BINARY_DIR}/${JLE_GAME_BUILD})
    FILE(COPY EngineResources DESTINATION ${PROJECT_BINARY_DIR}/${JLE_GAME_BUILD})
endif ()

if (JLE_BUILD_UNITY)
    set_target_properties(engine PROPERTIES
            UNITY_BUILD ON
            UNITY_BUILD_BATCH_SIZE 8)
endif ()

if (JLE_BUILD_PRECOMPILED_HEADER)
    target_precompile_headers(engine PRIVATE precompile.h)
endif ()