// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleRenderingFactoryInterface.h"

class jleRenderingFactory_OpenGL : public jleRenderingFactoryInterface {
public:
    [[nodiscard]] std::unique_ptr<jleRenderingAPIInterface> CreateRenderingAPI()
        const override;

    [[nodiscard]] std::unique_ptr<jleQuadRenderingInterface> CreateQuadRendering()
        const override;

    [[nodiscard]] std::unique_ptr<jleTextRenderingInterface> CreateTextRendering()
        const override;

    [[nodiscard]] std::unique_ptr<jleFramebufferInterface> CreateFramebuffer(
        unsigned int width, unsigned int height) const override;

    [[nodiscard]] std::unique_ptr<jleFullscreenRenderingInterface>
    CreateFullscreenRendering() const override;
};
