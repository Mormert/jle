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

#include "modules/jleGameModules.h"

#include <memory>
#include <vector>

struct jleSerializationContext;
class jleSerializableInterface;

namespace jlECS
{
class ECS;
class ObjectRef;
struct CreateComponentData;
struct DestroyComponentData;
}

class jleLuaEnvironment;
class cLuaScript;

class jleLuaModule : public jleGameBaseModule
{
public:
    jleLuaModule();

    virtual ~jleLuaModule();

    virtual void initializeECS(jlECS::ECS &ecs);

    virtual void initializeModule(jleSerializationContext& serializationContext);

    void populateSerializeableInterface(std::vector<jleSerializableInterface*>& interfaces);

    struct UpdateContext {
        struct In {
            float dt;
        } in;

        struct InOut {
            jlECS::ECS &ecs;
        } inOut;
    };

    void update(const UpdateContext &ctx);

    [[nodiscard]] jleLuaEnvironment& getEnvironment() const { return *_luaEnvironment; }

protected:
    void onLuaComponentCreated(const jlECS::CreateComponentData &createCallbackData);
    void onLuaComponentDestroyed(const jlECS::DestroyComponentData &destroyCallbackData);
    void onLuaComponentCopied(cLuaScript *source, cLuaScript *dest);

    std::unique_ptr<jleLuaEnvironment> _luaEnvironment{};
};
