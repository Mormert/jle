// Copyright (c) 2022. Johan Lind

#include "oCharacterHealthBar.h"

void oCharacterHealthBar::SetupDefaultObject() {
    mTransform = AddCustomComponent<cTransform>();
    mAseprite = AddCustomComponent<jle::cAseprite>();
}

void oCharacterHealthBar::SetHP(int maxHP, int currentHP) {
    const auto w = (currentHP / (float)maxHP) * (float)mMaxWidth;
    mAseprite->mWidth = w;
}