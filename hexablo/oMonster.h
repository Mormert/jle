// Copyright (c) 2022. Johan Lind

#pragma once

#include "oCharacter.h"

class oMonster : public oCharacter {
    JLE_REGISTER_OBJECT_TYPE(oMonster)
public:
    void upDefaultObject() override;

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json& j_out) override;

    void fromJson(const nlohmann::json& j_in) override;

private:
    void lookAtPlayer();
};
