// Copyright (c) 2022. Johan Lind

#include "oCharacterHealthBar.h"

void oCharacterHealthBar::upDefaultObject() {
    _transform = addCustomComponent<cTransform>();
    _aseprite = addCustomComponent<cAseprite>();
}

void oCharacterHealthBar::hp(int maxHP, int currentHP) {
    const auto w = (currentHP / (float)maxHP) * (float)_maxWidth;
    _aseprite->_width = w;
}
