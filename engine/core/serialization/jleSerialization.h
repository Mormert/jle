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

#include <utility>
#include <vector>

class jleResourceHolder;
class jleLuaEnvironment;
class jleRenderThread;

class jleSerializationContextInterface
{
};

struct jleSerializationContext {
    jleSerializationContext() = default;

    jleSerializationContext(jleResourceHolder *r, jleLuaEnvironment *l, jleRenderThread *rt)
        : resources(r), luaEnvironment(l), renderThread(rt)
    {
    }

    jleSerializationContext(jleResourceHolder *r, std::vector<jleSerializationContextInterface *> interfaces)
        : resources(r), serializationInterfaces(std::move(interfaces))
    {
    }

    // Optional, need to be null checked
    jleResourceHolder *resources{nullptr};

    std::vector<jleSerializationContextInterface *> serializationInterfaces;

    template <typename T>
    T* get()
    {
        for (auto *interface : serializationInterfaces) {
            if (T *interfaceCast = dynamic_cast<T *>(interface)) {
                return interfaceCast;
            }
        }
        return nullptr;
    }

    // Optional, need to be null checked
    jleLuaEnvironment *luaEnvironment{nullptr};

    // Optional, need to be null checked
    jleRenderThread *renderThread{nullptr};
};

class jleSerializationArchive
{
public:
    explicit jleSerializationArchive(jleSerializationContext &context) : ctx{context} {}

    jleSerializationContext ctx;
};

