// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleRenderingFactoryInterface.h"

class jleRenderingFactory_OpenGL : public jleRenderingFactoryInterface {
public:
    [[nodiscard]] std::unique_ptr<jleRendering> CreateRenderingAPI()
        const override;

    [[nodiscard]] std::unique_ptr<jleQuadRendering> CreateQuadRendering()
        const override;

    [[nodiscard]] std::unique_ptr<jleTextRendering> CreateTextRendering()
        const override;

    [[nodiscard]] std::unique_ptr<jleFramebufferInterface> CreateFramebuffer(
        unsigned int width, unsigned int height) const override;

    [[nodiscard]] std::unique_ptr<jleFullscreenRenderingInterface>
    CreateFullscreenRendering() const override;
};
