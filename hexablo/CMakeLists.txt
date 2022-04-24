
add_executable(hexablo
        "GameObject.cpp"
        "Hexablo.cpp"
        "main.cpp"
        "HexMap.cpp"
        "ObjectManager.cpp"
        )


target_link_libraries(${JLE_GAME_BUILD} LINK_PUBLIC engine)

include(${CMAKE_CURRENT_SOURCE_DIR}/../engine/cmake/game.cmake)