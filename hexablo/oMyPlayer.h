// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_OMYPLAYER_H
#define HEXABLO_OMYPLAYER_H

#include "oCharacter.h"

#include <iostream>
#include <memory>

class oMyPlayer : public oCharacter {
    JLE_REGISTER_OBJECT_TYPE(oMyPlayer)
public:
    void SetupDefaultObject() override;

    void Start() override;

    void Update(float dt) override;

    void ToJson(nlohmann::json& j_out) override;

    void FromJson(const nlohmann::json& j_in) override;

    static inline std::weak_ptr<oMyPlayer> sMyPlayerPtr;

    void Attack(oCharacterDirection direction) override;

    void Abilities();

private:
    void LookAtMouse();

    void Movement_v1(float dt);
    void Movement(float dt);

    bool mCanThrowFireball = true;
};

#endif // HEXABLO_OMYPLAYER_H
