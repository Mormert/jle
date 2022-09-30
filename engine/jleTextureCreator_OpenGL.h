// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleTextureCreatorInterface.h"

class jleTextureCreator_OpenGL final : public jleTextureCreatorInterface {
public:
    ~jleTextureCreator_OpenGL() override = default;

    std::unique_ptr<jleTexture> CreateTextureFromImage(
            const jleImage &image) override;
};