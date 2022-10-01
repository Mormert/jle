// Copyright (c) 2022. Johan Lind

#include "oStaticSprite.h"
#include "cAseprite.h"
#include "jleCore.h"
#include "jleScene.h"

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

    auto spawnedObject = SpawnChildObject<jleObject>();

    auto t = spawnedObject->AddCustomComponent<cTransform>();
    auto c = spawnedObject->AddCustomComponent<cAseprite>();
    c->AddAsepritePath("GR:rottenapple.json");
    c->_animating = true;
    c->_height = 12;
    c->_width = 12;

    t->SetLocalPosition(25 + (std::rand() % (325 - 25 + 1)),
                        25 + (std::rand() % (325 - 25 + 1)));
}

void oStaticSprite::Start() {}

void oStaticSprite::Update(float dt) {
    if (jleCore::core->input->keyboard->GetKeyDown('Q')) {
        auto spawnedObject = SpawnChildObject<jleObject>();

        auto t = spawnedObject->AddCustomComponent<cTransform>();
        auto c = spawnedObject->AddCustomComponent<cAseprite>();
        c->AddAsepritePath("GR:rottenapple.json");
        c->_animating = true;
        c->_height = 12;
        c->_width = 12;

        t->SetLocalPosition(25 + (std::rand() % (325 - 25 + 1)),
                            25 + (std::rand() % (325 - 25 + 1)));
    }
}
