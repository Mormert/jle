// Copyright (c) 2022. Johan Lind

#include "oCharacterHealthBar.h"

void oCharacterHealthBar::SetupDefaultObject() {
    _transform = AddCustomComponent<cTransform>();
    _aseprite = AddCustomComponent<cAseprite>();
}

void oCharacterHealthBar::SetHP(int maxHP, int currentHP) {
    const auto w = (currentHP / (float)maxHP) * (float)_maxWidth;
    _aseprite->_width = w;
}