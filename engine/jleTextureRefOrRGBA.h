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

#include "jleBuildConfig.h"

#include "jleCompileHelper.h"
#include "jleResourceRef.h"
#include "jleTexture.h"

#include <glm/glm.hpp>

class jleTextureRefOrRGBA
{
public:
    template <class Archive>
    void serialize(Archive &ar);

    explicit jleTextureRefOrRGBA(const glm::vec4 &color);

    [[nodiscard]] glm::vec4 &color();

    [[nodiscard]] jleResourceRef<jleTexture> &textureRef();

private:
    glm::vec4 _color{1.f};
    jleResourceRef<jleTexture> _textureRef;
};

class jleTextureRefOrRGB
{
public:
    template <class Archive>
    void serialize(Archive &ar);

    explicit jleTextureRefOrRGB(const glm::vec3 &color);

    [[nodiscard]] glm::vec3 &color();

    [[nodiscard]] jleResourceRef<jleTexture> &textureRef();

private:
    glm::vec3 _color{1.f};
    jleResourceRef<jleTexture> _textureRef;
};

class jleTextureRefOrAlpha
{
public:
    template <class Archive>
    void serialize(Archive &ar);

    explicit jleTextureRefOrAlpha(float alpha);

    [[nodiscard]] float &alpha();

    [[nodiscard]] jleResourceRef<jleTexture> &textureRef();

private:
    float _alpha{0.f};
    jleResourceRef<jleTexture> _textureRef;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleTextureRefOrRGBA)
JLE_EXTERN_TEMPLATE_CEREAL_H(jleTextureRefOrRGB)
JLE_EXTERN_TEMPLATE_CEREAL_H(jleTextureRefOrAlpha)