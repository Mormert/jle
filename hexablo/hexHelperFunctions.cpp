// Copyright (c) 2022. Johan Lind

#include "hexHelperFunctions.h"
#include "jleCore.h"
#include "jleGameEngine.h"

int hexHelperFunctions::GetPixelatedMouseXWorldSpace() {
    auto& game = ((jleGameEngine *)jleCore::core)->GetGameRef();
    return jleCore::core->input->mouse->GetPixelatedMouseX() +
           game._mainCamera.GetIntX();
}

int hexHelperFunctions::GetPixelatedMouseYWorldSpace() {
    auto& game = ((jleGameEngine *)jleCore::core)->GetGameRef();
    return jleCore::core->input->mouse->GetPixelatedMouseY() +
           game._mainCamera.GetIntY();
}

int hexHelperFunctions::GetRandInt(int min, int max) {
    int range = max - min + 1;
    return std::rand() % range + min;
}
