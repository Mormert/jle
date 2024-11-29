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

// Classes that should be accessible in a jleSerializationArchive should derive from this interface
class jleSerializableInterface
{
public:
    virtual ~jleSerializableInterface() = default;
};

struct jleSerializationContext {
    jleResourceHolder *resources{nullptr};
    const std::vector<jleSerializableInterface *> serializationInterfaces = {};

    template <typename T>
    T* get()
    {
        static_assert(std::is_base_of<jleSerializableInterface, T>());
        for (auto *interface : serializationInterfaces) {
            if (T *interfaceCast = dynamic_cast<T *>(interface)) {
                return interfaceCast;
            }
        }
        return nullptr;
    }
};

class jleSerializationArchive
{
public:
    explicit jleSerializationArchive(jleSerializationContext &context) : ctx{context} {}

    jleSerializationContext ctx;
};

