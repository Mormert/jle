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

#include "jleLuaModule.h"

#include <jlECS/jlECS.h>
#include "components/cLuaScript.h"


namespace {
void
deep_copy(sol::state &lua, const sol::table &src, sol::table &dest)
{
    if (src.is<sol::table>()) {
        sol::table src_table = src;
        for (auto const &kv_pair : src_table) {
            sol::object key = kv_pair.first;
            sol::object value = kv_pair.second;
            if (value.is<sol::table>()) {
                sol::table nested_dest_table = lua.create_table();
                deep_copy(lua, value, nested_dest_table);
                dest[key] = nested_dest_table;
            } else {
                dest[key] = value;
            }
        }
    }
}
}

jleLuaModule::jleLuaModule() = default;
jleLuaModule::~jleLuaModule() = default;

void
jleLuaModule::initializeECS(jlECS::ECS &ecs)
{
    ecs.registerComponentType<cLuaScript>({
        .onCreateCallback = [this](jlECS::CreateComponentData& data) {
            onLuaComponentCreated(data);
        },
        .onDestroyCallback = [this](jlECS::DestroyComponentData& data) {
            onLuaComponentDestroyed(data);
        },
        .onDuplicateCallback = [this](void* sourceComp, void* destComp) {
            onLuaComponentCopied(static_cast<cLuaScript*>(sourceComp), static_cast<cLuaScript*>(destComp));
        }
    });
}

void jleLuaModule::initializeModule(jleSerializationContext &serializationContext) {
    _luaEnvironment = std::make_unique<jleLuaEnvironment>();

    _luaEnvironment->loadScript(JLE_PATH_HASH("ER:/scripts/engine.lua"), serializationContext);
    _luaEnvironment->loadScript(JLE_PATH_HASH("ER:/scripts/globals.lua"), serializationContext);

    _luaEnvironment->loadInitialScripts(serializationContext);
}

void
jleLuaModule::populateSerializeableInterface(std::vector<jleSerializableInterface *> &interfaces)
{
    jleLuaEnvironment* luaEnvironment = _luaEnvironment.get();
    jleAssert(luaEnvironment);
    interfaces.push_back(luaEnvironment);
}

void
jleLuaModule::update(const UpdateContext &ctx)
{
    auto& luaEnvironment = *_luaEnvironment;
    for (auto& scriptComponent : ctx.inOut.ecs.iterate<cLuaScript>()) {
        scriptComponent.update(luaEnvironment, ctx.in.dt);
    }
}

void
jleLuaModule::onLuaComponentCreated(const jlECS::CreateComponentData &createCallbackData)
{
    cLuaScript* ptr = static_cast<cLuaScript*>(createCallbackData.componentPtr);
    ptr->start(*_luaEnvironment);
}

void
jleLuaModule::onLuaComponentDestroyed(const jlECS::DestroyComponentData& destroyCallbackData)
{
    cLuaScript* ptr = static_cast<cLuaScript*>(destroyCallbackData.componentPtr);
    ptr->onDestroy(*_luaEnvironment);
}

void
jleLuaModule::onLuaComponentCopied(cLuaScript *source, cLuaScript *dest)
{
    dest->_luaComponent = source->_luaComponent;

    if (source->_isInitialized) {
        dest->initializeLuaComponent(*_luaEnvironment);
        deep_copy(_luaEnvironment->getState(), source->getSelf(), dest->getSelf());
    }
}

