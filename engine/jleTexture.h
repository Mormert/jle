// Copyright (c) 2022. Johan Lind

#pragma once

#include "no_copy_no_move.h"

#include <climits>
#include <memory>
#include <string>

class jleImage;

class jleTexture {
    NO_COPY_NO_MOVE(jleTexture)

public:
    jleTexture() = default;
    explicit jleTexture(const jleImage& image);

    ~jleTexture();

    // Returns true if this Texture is the globally active texture
    bool IsActive();

    // Set this Texture to be the globally active texture
    void SetToActiveTexture(int texture_slot = 0);

    unsigned int GetTextureID();

    int32_t GetWidth();

    int32_t GetHeight();

    unsigned int id();

private:
    int32_t _width = 0, _height = 0, _nrChannels = 0;
    unsigned int _id = UINT_MAX; // OpenGL Texture ID
};
