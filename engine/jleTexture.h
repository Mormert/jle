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

#include "jlePath.h"
#include "jleSerializedResource.h"
#include "jleTypeReflectionUtils.h"
#include "jleTextureRules.h"

#include <climits>

#include <cereal/cereal.hpp>

class jleTexture : public jleSerializedResource, public std::enable_shared_from_this<jleTexture>
{
public:
    jleTexture() = default;

    JLE_REGISTER_RESOURCE_TYPE(jleTexture, "tex", "png", "jpg", "tga", "bmp", "psd", "jpeg")

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(imagePath));
    }

    [[nodiscard]] bool loadFromFile(const jlePath &path) override;

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedResource)

    ~jleTexture() override;

    void setActive(jleTextureSlot slot);

    int32_t width();

    int32_t height();

    unsigned int id();

    [[nodiscard]] const int channels();

    jlePath imagePath;

private:
    int32_t _width = 0, _height = 0, _nrChannels = 0;
    unsigned int _id = UINT_MAX; // OpenGL Texture ID
};

CEREAL_REGISTER_TYPE(jleTexture)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleTexture)