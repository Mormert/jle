// Copyright (c) 2022. Johan Lind

#include "oStaticSprite.h"

void oStaticSprite::SetupDefaultObject() {
    transform = AddComponent<cTransform>();
    sprite = AddComponent<cSprite>();

    sprite->CreateAndSetTextureFromPath("GameResources/Fruit+.png");

    beginX += 16;
    if (beginX > 608) {
        beginX -= 608;
    }
    beginY += 16;
    if (beginY > 96) {
        beginY -= 96;
    }

    sprite->SetTextureBeginCoordinates(beginX, beginY);
    sprite->SetRectangleDimensions(16, 16);
}

void oStaticSprite::Start() {
}

void oStaticSprite::Update(float dt) {
}
