# Copyright (c) 2022. Johan Lind

add_executable(hexablo
        "Hexablo.cpp"
        "main.cpp"
        oWorld.cpp oWorld.h hexHexagonFunctions.cpp hexHexagonFunctions.h hexHelperFunctions.cpp hexHelperFunctions.h oCharacter.cpp oCharacter.h)


target_link_libraries(${JLE_GAME_BUILD} LINK_PUBLIC engine)

include(${CMAKE_CURRENT_SOURCE_DIR}/../engine/cmake/game.cmake)