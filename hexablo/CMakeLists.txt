# Copyright (c) 2022. Johan Lind

add_executable(hexablo
#        "Hexablo.cpp"
#        "main.cpp"
#        "oWorld.cpp"
#        "hexHexagonFunctions.cpp"
#        "hexHelperFunctions.cpp"
#        "oCharacter.cpp"
#        "oMyPlayer.cpp"
#        "hexHexagonItem.cpp"
#        "oCharacterHealthBar.cpp"
#        "oMonster.cpp"
#        "hexNetScene.cpp"
#        "oFireball.cpp"
         ../hexablo.cpp
        )


target_link_libraries(${JLE_GAME_BUILD} LINK_PUBLIC engine)

target_include_directories(hexablo SYSTEM PUBLIC)

include(${CMAKE_CURRENT_SOURCE_DIR}/../engine/cmake/game.cmake)

