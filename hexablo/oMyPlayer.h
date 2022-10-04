// Copyright (c) 2022. Johan Lind

#pragma once

#include "oCharacter.h"

#include <iostream>
#include <memory>

class oMyPlayer : public oCharacter {
    JLE_REGISTER_OBJECT_TYPE(oMyPlayer)
public:
    void setupDefaultObject() override;

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

    static inline std::weak_ptr<oMyPlayer> sMyPlayerPtr;

    void attack(oCharacterDirection direction) override;

    void abilities();

private:
    void lookAtMouse();

    void Movement_v1(float dt);
    void movement(float dt);

    bool _canThrowFireball = true;
};
