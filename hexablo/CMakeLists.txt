# Copyright (c) 2022. Johan Lind


set(FASTNOISE2_NOISETOOL OFF CACHE BOOL "Build Noise Tool" FORCE) #example if don't need the graph tool

add_subdirectory(3rdparty/FastNoise2)

add_executable(hexablo
        "Hexablo.cpp"
        "main.cpp"
        "oWorld.cpp"
        "hexHexagonFunctions.cpp"
        "hexHelperFunctions.cpp"
        "oCharacter.cpp"
        "oMyPlayer.cpp"
        "hexHexagonItem.cpp"
        ${source})


target_link_libraries(${JLE_GAME_BUILD} LINK_PUBLIC engine FastNoise)

target_include_directories(hexablo SYSTEM PUBLIC 3rdparty/FastNoise2/include)

include(${CMAKE_CURRENT_SOURCE_DIR}/../engine/cmake/game.cmake)