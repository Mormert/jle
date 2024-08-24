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

#include "jleResourceInterface.h"

#define SAVE_SHARED_THIS_SERIALIZED_JSON(PARENT_TYPE)                                                                  \
    void saveToFile(jleSerializationContext &ctx) override                                                             \
    {                                                                                                                  \
        std::ofstream save{path.getRealPath()};                                                                        \
        jleJSONOutputArchive outputArchive(save, ctx);                                                         \
        std::shared_ptr<PARENT_TYPE> thiz = std::static_pointer_cast<PARENT_TYPE>(shared_from_this());                 \
        outputArchive(thiz);                                                                                           \
    };

// Derive from this whenever a resource relies on save/load from serialization archives, but also implements a
// loadFromFile() member function.
class jleSerializedResource : public jleResourceInterface
{
public:
    jleSerializedResource() = default;

    ~jleSerializedResource() override = default;

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        // Empty serialization function
    }
};

CEREAL_REGISTER_TYPE(jleSerializedResource)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleResourceInterface, jleSerializedResource)

// Derive from this whenever a resource only relies on save/load via serialization archives
class jleSerializedOnlyResource : public jleSerializedResource
{
public:
    jleSerializedOnlyResource() = default;

    ~jleSerializedOnlyResource() override = default;

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        // Empty serialization function
    }

    bool
    loadFromFile(const jlePath &path) final
    {
        // This empty interface function should never be called, instead the macro
        // SAVE_SHARED_THIS_SERIALIZED_JSON should be used!
        LOGE << "loadFromFile called for serialized-only resource, which should not happen.";
        return false;
    }
};

CEREAL_REGISTER_TYPE(jleSerializedOnlyResource)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedResource, jleSerializedOnlyResource)