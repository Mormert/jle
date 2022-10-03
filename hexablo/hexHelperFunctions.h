// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_HEXHELPERFUNCTIONS_H
#define HEXABLO_HEXHELPERFUNCTIONS_H

class hexHelperFunctions {
public:
    [[nodiscard]] static int GetPixelatedMouseXWorldSpace();
    [[nodiscard]] static int GetPixelatedMouseYWorldSpace();

    [[nodiscard]] static int GetRandInt(int min, int max);
};

#endif // HEXABLO_HEXHELPERFUNCTIONS_H
