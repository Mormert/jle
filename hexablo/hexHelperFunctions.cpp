// Copyright (c) 2022. Johan Lind

#include "hexHelperFunctions.h"
#include "jleCore.h"
#include "jleGameEngine.h"

int hexHelperFunctions::pixelatedMouseXWorldSpace() {
    auto &game = ((jleGameEngine *)gCore)->gameRef();
    return gCore->input().mouse->pixelatedMouseX() + game._mainCamera.intX();
}

int hexHelperFunctions::pixelatedMouseYWorldSpace() {
    auto &game = ((jleGameEngine *)gCore)->gameRef();
    return gCore->input().mouse->pixelatedMouseY() + game._mainCamera.intY();
}

int hexHelperFunctions::randInt(int min, int max) {
    int range = max - min + 1;
    return std::rand() % range + min;
}
