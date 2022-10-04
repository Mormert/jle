// Copyright (c) 2022. Johan Lind

#pragma once

#include "jlePath.h"
#include <climits>
#include <memory>
#include <string>

class jleImage;

class jleTexture {
public:
    jleTexture(const jleTexture&) = delete;
    jleTexture& operator=(const jleTexture&) = delete;
    jleTexture(jleTexture&& e) = delete;
    jleTexture& operator=(jleTexture&& e) = delete;

    jleTexture() = default;
    explicit jleTexture(const jleImage& image);

    // Utilizes the resource holder such that only one copy of the resource will
    // exists, and if it does not exist, the program will construct it and store
    // it for later use
    static std::shared_ptr<jleTexture> fromPath(const jleRelativePath& path);

    ~jleTexture();

    // Returns true if this Texture is the globally active texture
    bool isActive();

    // Set this Texture to be the globally active texture
    void toActiveTexture(int texture_slot = 0);

    int32_t width();

    int32_t height();

    unsigned int id();

private:
    int32_t _width = 0, _height = 0, _nrChannels = 0;
    unsigned int _id = UINT_MAX; // OpenGL Texture ID
};
