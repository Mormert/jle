// Copyright (c) 2022. Johan Lind

#include "hexHelperFunctions.h"
#include "jleCore.h"
#include "jleGameEngine.h"

int hexHelperFunctions::GetPixelatedMouseXWorldSpace() {
    auto &game = ((jle::jleGameEngine *) jle::jleCore::core)->GetGameRef();
    return jle::jleCore::core->input->mouse->GetPixelatedMouseX() + game.mMainCamera.GetIntX();
}

int hexHelperFunctions::GetPixelatedMouseYWorldSpace() {
    auto &game = ((jle::jleGameEngine *) jle::jleCore::core)->GetGameRef();
    return jle::jleCore::core->input->mouse->GetPixelatedMouseY() + game.mMainCamera.GetIntY();
}

int hexHelperFunctions::GetRandInt(int min, int max) {
    int range = max - min + 1;
    return std::rand() % range + min;
}
