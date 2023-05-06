// Copyright (c) 2023. Johan Lind

#pragma once

#include "jlePath.h"
#include "jleSerializedResource.h"
#include "jleTypeReflectionUtils.h"

#include <climits>

#include <cereal/cereal.hpp>

class jleTexture : public jleSerializedResource, public std::enable_shared_from_this<jleTexture>
{
public:
    jleTexture() = default;

    JLE_REGISTER_RESOURCE_TYPE(jleTexture, tex)

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(imagePath));
    }

    jleLoadFromFileSuccessCode loadFromFile(const jlePath &path) override;

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedResource)

    ~jleTexture() override;

    // Returns true if this Texture is the globally active texture
    bool isActive();

    // Set this Texture to be the globally active texture
    void setActive(int texture_slot = 0);

    int32_t width();

    int32_t height();

    unsigned int id();

    jlePath imagePath;

private:
    int32_t _width = 0, _height = 0, _nrChannels = 0;
    unsigned int _id = UINT_MAX; // OpenGL Texture ID
};

CEREAL_REGISTER_TYPE(jleTexture)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedResource, jleTexture)