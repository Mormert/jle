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

#include <memory>
#include <vector>

class jleSerializableInterface;
struct jleSerializationContext;
namespace jlECS
{
class ECS;
}
struct jleEngineUpdateContext;
struct jleGameBaseModule {
    virtual ~jleGameBaseModule() = default;
};

struct jleGameModules
{
    template <class T>
    class ModuleNum
    {
    public:
        static inline uint16_t num = UINT16_MAX;
    };

    class ModuleIDGenerator {
    public:
        static uint16_t nextID() {
            static uint16_t current = 0;
            return current++;
        }
    };

    virtual ~jleGameModules() = default;

    std::vector<std::unique_ptr<jleGameBaseModule>> modules;

    template <typename T>
    T* getModule()
    {
        static_assert(std::is_base_of<jleGameBaseModule, T>::value, "Module must derive from jleGameBaseModule");

        if (ModuleNum<T>::num == UINT16_MAX) {
            return nullptr;
        }

        return static_cast<T*>(modules[ModuleNum<T>::num].get());
    }

    template <typename Primary, typename... Aliases>
    void addModule(std::unique_ptr<Primary> module)
    {
        static_assert(std::is_base_of<jleGameBaseModule, Primary>::value, "Module must derive from jleGameBaseModule");

        uint16_t index = static_cast<uint16_t>(modules.size());
        modules.push_back(std::move(module));

        // Assign index to Primary and any alias types
        ModuleNum<Primary>::num = index;
        (void)std::initializer_list<int>{
            (ModuleNum<Aliases>::num = index, 0)...
        };
    }

    virtual void initialize(jlECS::ECS &ecs, jleSerializationContext& serializationContext);
    virtual void update(jleEngineUpdateContext& ctx, jlECS::ECS& ecs);
    virtual void updateRenderablesOnly(jleEngineUpdateContext& ctx, jlECS::ECS& ecs);
    virtual void populateSerializeableInterfaces(std::vector<jleSerializableInterface*>& interfaces);
};