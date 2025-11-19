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
#include <cassert>

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
    template <typename T>
    T* getModule()
    {
        static_assert(std::is_base_of<jleGameBaseModule, T>::value, "Module must derive from jleGameBaseModule");
        auto moduleNum = ModuleNum<T>::num;
        if (moduleNum == UINT16_MAX) {
            return nullptr;
        }

        return static_cast<T*>(_modules[moduleNum].get());
    }

    template <typename Primary, typename... Aliases>
    void addModule(std::unique_ptr<Primary> module)
    {
        static_assert(std::is_base_of<jleGameBaseModule, Primary>::value, "Module must derive from jleGameBaseModule");

        uint16_t index = static_cast<uint16_t>(_modules.size());

        if (ModuleNum<Primary>::num != UINT16_MAX) {
            assert(ModuleNum<Primary>::num == index && "Module index mismatch from previous registration. Make sure to register modules in the same order.");
        }

        (void)std::initializer_list<int>{
            (ModuleNum<Aliases>::num != UINT16_MAX
                ? (assert(ModuleNum<Aliases>::num == index && "Module index mismatch from previous registration. Make sure to register modules in the same order."), 0)
                : 0)...
        };

        _modules.push_back(std::move(module));

        ModuleNum<Primary>::num = index;
        (void)std::initializer_list<int>{
            (ModuleNum<Aliases>::num = index, 0)...
        };
    }

private:
    template <class T>
    class ModuleNum
    {
    public:
        static inline uint16_t num = UINT16_MAX;
    };

    std::vector<std::unique_ptr<jleGameBaseModule>> _modules;
};