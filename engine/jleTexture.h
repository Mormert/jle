// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleTextureInterface.h"
#include "no_copy_no_move.h"

#include <climits>
#include <string>

class jleTexture : public jleTextureInterface {
    NO_COPY_NO_MOVE(jleTexture)

public:
    jleTexture() = default;

    ~jleTexture() override;

    // Returns true if this Texture is the globally active texture
    bool IsActive() override;

    // Set this Texture to be the globally active texture
    void SetToActiveTexture(int texture_slot = 0) override;

    unsigned int GetTextureID() override;

private:
    friend class jleTextureCreator_OpenGL;

    unsigned int texture_id = UINT_MAX; // OpenGL Texture ID
};
