
add_executable (Pong2
    "cSprite.cpp"
    "cTransform.cpp"
    "main.cpp"
    "oPlayerRacket.cpp"
    "oRacket.cpp"
    "oStaticSprite.cpp"
    "Pong2.cpp"
)


target_link_libraries (Pong2 LINK_PUBLIC engine)