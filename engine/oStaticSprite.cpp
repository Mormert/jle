// Copyright (c) 2022. Johan Lind

#include "oStaticSprite.h"
#include "cAseprite.h"
#include "jleCore.h"
#include "jleScene.h"

void oStaticSprite::setupDefaultObject() {
    transform = addComponent<cTransform>();
    sprite = addComponent<cSprite>();

    sprite->createAndSetTextureFromPath("GameResources/Fruit+.png");

    beginX += 16;
    if (beginX > 608) {
        beginX -= 608;
    }
    beginY += 16;
    if (beginY > 96) {
        beginY -= 96;
    }

    sprite->textureBeginCoordinates(beginX, beginY);
    sprite->rectangleDimensions(16, 16);

    auto spawnedObject = spawnChildObject<jleObject>();

    auto t = spawnedObject->addCustomComponent<cTransform>();
    auto c = spawnedObject->addCustomComponent<cAseprite>();
    c->addAsepritePath("GR:rottenapple.json");
    c->_animating = true;
    c->_height = 12;
    c->_width = 12;

    t->setLocalPosition({25 + (std::rand() % (325 - 25 + 1)), 25 + (std::rand() % (325 - 25 + 1)), 0.f});
}

void oStaticSprite::start() {}

void oStaticSprite::update(float dt) {
    if (gCore->input().keyboard->keyDown('Q')) {
        auto spawnedObject = spawnChildObject<jleObject>();

        auto t = spawnedObject->addCustomComponent<cTransform>();
        auto c = spawnedObject->addCustomComponent<cAseprite>();
        c->addAsepritePath("GR:rottenapple.json");
        c->_animating = true;
        c->_height = 12;
        c->_width = 12;

        t->setLocalPosition({25 + (std::rand() % (325 - 25 + 1)), 25 + (std::rand() % (325 - 25 + 1)), 0.f});
    }
}
