#pragma once

#include "jleGame.h"

class Hexablo final : public jleGame {
public:
    ~Hexablo() override = default;

    void start() override;

    void update(float dt) override;
};