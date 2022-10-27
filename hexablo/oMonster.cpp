// Copyright (c) 2022. Johan Lind

#include "oMonster.h"
#include "oMyPlayer.h"
#include <memory>

void oMonster::setupDefaultObject() { oCharacter::setupDefaultObject(); }

void oMonster::start() { oCharacter::start(); }

void oMonster::update(float dt) {
    oCharacter::update(dt);

    lookAtPlayer();
}

void oMonster::toJson(nlohmann::json &j_out) { oCharacter::toJson(j_out); }

void oMonster::fromJson(const nlohmann::json &j_in) {
    oCharacter::fromJson(j_in);
}

void oMonster::lookAtPlayer() {

    if (auto &&p = oMyPlayer::sMyPlayerPtr.lock()) {
        auto &&transform = p->component<cTransform>();
        lookAtPosition(transform->worldPosition().x,
                       transform->worldPosition().y);
    }
}
