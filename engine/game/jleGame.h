/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#pragma once

#include "core/jleCommon.h"

#include <memory>
#include <vector>

#include "core/jleCamera.h"
#include "core/jleProfiler.h"
#include "core/jleResourceHolder.h"
#include "core/jleScene.h"
#include "jlECS/jlECS.h"

#include <modules/jleEngineUpdateContext.h>
#include <modules/graphics/jleGraphicsModule.h>
#include <modules/physics/jlePhysicsModule.h>
#include <modules/hierarchy/jleHierarchyModule.h>


#include <execution>
#include <fstream>
#include <iostream>
#include <typeinfo>

struct jleEngineUpdateContext;

struct jleGameState
{
    std::unique_ptr<jlECS::ECS> ecs;
    std::unique_ptr<jlePhysics> physics;
};

struct jleGameModules{
    std::unique_ptr<jleHierarchyModule> coreModule;
    std::unique_ptr<jleGraphicsModule> graphicsModule;
    std::unique_ptr<jlePhysicsModule> physicsModule;
};

class jleGame
{
public:
    jleGame();
    virtual ~jleGame();

    struct GameStartContext{
        std::unique_ptr<jlECS::ECS> ecs;
    };

    void injectModules(std::unique_ptr<jleGameModules> modules);
    jleGameModules& getModules() const { return *_modules; }

    virtual void update(jleEngineUpdateContext &ctx);
    virtual void start(GameStartContext& ctx);

    [[nodiscard]] jleGameState& getGameState() { return _gameState; }

protected:
    std::unique_ptr<jleGameModules> _modules;

    jleGameState _gameState{};

};