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
#include <modules/scripting/jleLuaModule.h>


#include <execution>
#include <fstream>
#include <iostream>
#include <typeinfo>

struct jleEngineUpdateContext;

struct jleGameModules
{
    std::unique_ptr<jleHierarchyModule> hierarchyModule;
    std::unique_ptr<jleGraphicsModule> graphicsModule;
    std::unique_ptr<jlePhysicsModule> physicsModule;
    std::unique_ptr<jleLuaModule> luaModule;

    virtual void initialize(jlECS::ECS &ecs, jleSerializationContext& serializationContext);
    virtual void update(jleEngineUpdateContext& ctx, jlECS::ECS& ecs);
    virtual void updateRenderablesOnly(jleEngineUpdateContext& ctx, jlECS::ECS& ecs);
    virtual void populateSerializeableInterfaces(std::vector<jleSerializableInterface*>& interfaces);
};

class jleGame
{
public:
    jleGame();
    virtual ~jleGame();

    [[nodiscard]] jleGameModules& getModules() const { return *_modules; }

    void update(jleEngineUpdateContext &ctx);
    virtual void start(jleSerializationContext& serializationContext);

    [[nodiscard]] jlECS::ECS& getECS() { return *_ecs; }

protected:
    friend class jleGameRuntime;

    std::unique_ptr<jleGameModules> _modules;
    std::unique_ptr<jlECS::ECS> _ecs;
};