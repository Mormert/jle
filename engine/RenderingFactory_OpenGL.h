// Copyright (c) 2022. Johan Lind

#pragma once

#include "iRenderingFactory.h"

namespace jle {
    class RenderingFactory_OpenGL : public iRenderingFactory {
    public:
        [[nodiscard]] std::unique_ptr<iRenderingAPI> CreateRenderingAPI() const override;

        [[nodiscard]] std::unique_ptr<iQuadRendering> CreateQuadRendering() const override;

        [[nodiscard]] std::unique_ptr<iTextRendering> CreateTextRendering() const override;

        [[nodiscard]] std::unique_ptr<iFramebuffer>
        CreateFramebuffer(unsigned int width, unsigned int height) const override;

        [[nodiscard]] std::unique_ptr<iFullscreenRendering> CreateFullscreenRendering() const override;
    };
}