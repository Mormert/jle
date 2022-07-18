// Copyright (c) 2022. Johan Lind

#include "oMonster.h"
#include "oMyPlayer.h"
#include <memory>

void oMonster::SetupDefaultObject() {
    oCharacter::SetupDefaultObject();
}

void oMonster::Start() {
    oCharacter::Start();
}

void oMonster::Update(float dt) {
    oCharacter::Update(dt);

    LookAtPlayer();
}

void oMonster::ToJson(nlohmann::json &j_out) {
    oCharacter::ToJson(j_out);
}

void oMonster::FromJson(const nlohmann::json &j_in) {
    oCharacter::FromJson(j_in);
}

void oMonster::LookAtPlayer() {

    if (auto &&p = oMyPlayer::sMyPlayerPtr.lock()) {
        auto &&transform = p->GetComponent<cTransform>();
        LookAtPosition(transform->GetWorldX(), transform->GetWorldY());
    }

}
