// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFileLoadInterface.h"
#include "jlePath.h"

#include <cereal/cereal.hpp>

class jleTexture : public jleFileLoadInterface
{
public:
    jleTexture() = default;

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(imagePath));
    }

    bool loadFromFile(const std::string &path) override;

    void saveToFile() override;

    ~jleTexture() override;

    // Returns true if this Texture is the globally active texture
    bool isActive();

    // Set this Texture to be the globally active texture
    void setActive(int texture_slot = 0);

    int32_t width();

    int32_t height();

    unsigned int id();

    jleRelativePath imagePath;

private:
    int32_t _width = 0, _height = 0, _nrChannels = 0;
    unsigned int _id = UINT_MAX; // OpenGL Texture ID
};
