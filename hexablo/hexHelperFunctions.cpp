// Copyright (c) 2022. Johan Lind

#include "hexHelperFunctions.h"
#include "jleCore.h"
#include "jleGameEngine.h"

int hexHelperFunctions::pixelatedMouseXWorldSpace() {
    auto &game = ((jleGameEngine *)gCore)->gameRef();
    return gCore->input().mouse->pixelatedMouseX() + (int)game.mainCamera.getViewPosition().x;
}

int hexHelperFunctions::pixelatedMouseYWorldSpace() {
    auto &game = ((jleGameEngine *)gCore)->gameRef();
    return gCore->input().mouse->pixelatedMouseY() + (int)game.mainCamera.getViewPosition().y;
}

int hexHelperFunctions::randInt(int min, int max) {
    int range = max - min + 1;
    return std::rand() % range + min;
}
