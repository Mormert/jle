// Copyright (c) 2022. Johan Lind

#include "jleGame.h"

#pragma once

// Just an empty game template that can be started in editor or as a standalone program

class GameTemplate final : public jleGame
{
public:
    ~GameTemplate() override = default;

    void start() override;

    void update(float dt) override;
};
