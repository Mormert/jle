// Copyright (c) 2022. Johan Lind

#pragma once

class hexHelperFunctions {
public:
    [[nodiscard]] static int pixelatedMouseXWorldSpace();
    [[nodiscard]] static int pixelatedMouseYWorldSpace();

    [[nodiscard]] static int randInt(int min, int max);
};
