// Copyright (c) 2022. Johan Lind

#pragma once

#include "iTextureCreator.h"

namespace jle {
    class TextureCreator_OpenGL final : public iTextureCreator {
    public:
        ~TextureCreator_OpenGL() override = default;

        std::unique_ptr<iTexture> CreateTextureFromImage(const Image &image) override;
    };
}