#pragma once

#include "jleGame.h"

class Hexablo final : public jle::jleGame {
public:
    ~Hexablo() override = default;

    void Start() override;

    void Update(float dt) override;
};