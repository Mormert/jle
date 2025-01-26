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
#include "core/jleResourceHolder.h"
#include "jlECS/jlECS.h"

#include <modules/jleEngineUpdateContext.h>

#include "modules/jleGameModules.h"

struct jleEngineUpdateContext;

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