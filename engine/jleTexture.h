// Copyright (c) 2023. Johan Lind

#pragma once

#include "jlePath.h"
#include "jleResourceInterface.h"
#include <climits>

#include <cereal/cereal.hpp>

class jleTexture : public jleResourceInterface
{
public:
    jleTexture() = default;

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(imagePath));
    }

    jleLoadFromFileSuccessCode loadFromFile(const jlePath &path) override;

    void saveToFile() override;

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
