// Copyright (c) 2023. Johan Lind

#include "modules/game/jleGame.h"

#pragma once

// Just an empty game template that can be started in editor or as a standalone program

class GameTemplate final : public jleGame
{
public:
    ~GameTemplate() override = default;

    void start(jleEngineModulesContext& ctx) override;

    void update(jleEngineModulesContext& ctx) override;
};
