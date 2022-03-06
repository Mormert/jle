
add_executable (hexablo
        "GameObject.cpp"
        "Hexablo.cpp"
        "main.cpp"
        "HexMap.cpp"
        "ObjectManager.cpp"
        )


target_link_libraries (hexablo LINK_PUBLIC engine)

FILE(COPY GameResources DESTINATION ../hexablo)