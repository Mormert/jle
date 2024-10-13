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

#include "jleTextureRefOrRGBA.h"
#include "editor/jleImGuiArchive.h"
#include "serialization/jleExternalSerialization.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleTextureRefOrRGBA)
JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleTextureRefOrRGB)
JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleTextureRefOrAlpha)

jleTextureRefOrRGBA::jleTextureRefOrRGBA(const glm::vec4 &color) : _color{color} {}

glm::vec4 &
jleTextureRefOrRGBA::color()
{
    return _color;
}

jleResourceRef<jleTexture> &
jleTextureRefOrRGBA::textureRef()
{
    return _textureRef;
}

template <class Archive>
void
jleTextureRefOrRGBA::serialize(Archive &ar)
{
    ar(CEREAL_NVP(_color), CEREAL_NVP(_textureRef));
}

jleTextureRefOrRGB::jleTextureRefOrRGB(const glm::vec3 &color) : _color{color} {}

glm::vec3 &
jleTextureRefOrRGB::color()
{
    return _color;
}

jleResourceRef<jleTexture> &
jleTextureRefOrRGB::textureRef()
{
    return _textureRef;
}

template <class Archive>
void
jleTextureRefOrRGB::serialize(Archive &ar)
{
    ar(CEREAL_NVP(_color), CEREAL_NVP(_textureRef));
}

jleTextureRefOrAlpha::jleTextureRefOrAlpha(float alpha) : _alpha{alpha} {}

float &
jleTextureRefOrAlpha::alpha()
{
    return _alpha;
}

jleResourceRef<jleTexture> &
jleTextureRefOrAlpha::textureRef()
{
    return _textureRef;
}

template <class Archive>
void
jleTextureRefOrAlpha::serialize(Archive &ar)
{
    ar(CEREAL_NVP(_alpha), CEREAL_NVP(_textureRef));
}
