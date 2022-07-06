// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_OMYPLAYER_H
#define HEXABLO_OMYPLAYER_H

#include "oCharacter.h"

#include <memory>
#include <iostream>


class oMyPlayer : public oCharacter {
    JLE_REGISTER_OBJECT_TYPE(oMyPlayer)
public:

    void SetupDefaultObject() override;

    void Start() override;

    void Update(float dt) override;

    void ToJson(nlohmann::json &j_out) override;

    void FromJson(const nlohmann::json &j_in) override;

private:
    void LookAtMouse();

    void Movement(float dt);

};

#endif //HEXABLO_OMYPLAYER_H
