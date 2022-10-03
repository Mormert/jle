// Copyright (c) 2022. Johan Lind

#pragma once

class hexHelperFunctions {
public:
    [[nodiscard]] static int GetPixelatedMouseXWorldSpace();
    [[nodiscard]] static int GetPixelatedMouseYWorldSpace();

    [[nodiscard]] static int GetRandInt(int min, int max);
};
