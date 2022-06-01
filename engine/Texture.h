// Copyright (c) 2022. Johan Lind

#pragma once

#include "iTexture.h"
#include "no_copy_no_move.h"

#include <string>
#include <climits>

namespace jle {
    class Texture : public iTexture {
        NO_COPY_NO_MOVE(Texture)

    public:

        Texture() = default;

        ~Texture() override;

        // Returns true if this Texture is the globally active texture
        bool IsActive() override;

        // Set this Texture to be the globally active texture
        void SetToActiveTexture() override;

        unsigned int GetTextureID() override;

    private:
        friend class TextureCreator_OpenGL;

        unsigned int texture_id = UINT_MAX; // OpenGL Texture ID

    };

}
