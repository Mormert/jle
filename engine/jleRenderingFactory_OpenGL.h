// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleRenderingFactoryInterface.h"

class jleRenderingFactory_OpenGL : public jleRenderingFactoryInterface {
public:
    [[nodiscard]] std::unique_ptr<jleRenderingAPI_OpenGL> CreateRenderingAPI()
        const override;

    [[nodiscard]] std::unique_ptr<jleQuadRendering_OpenGL> CreateQuadRendering()
        const override;

    [[nodiscard]] std::unique_ptr<jleTextRendering_OpenGL> CreateTextRendering()
        const override;

    [[nodiscard]] std::unique_ptr<jleFramebufferInterface> CreateFramebuffer(
        unsigned int width, unsigned int height) const override;

    [[nodiscard]] std::unique_ptr<jleFullscreenRenderingInterface>
    CreateFullscreenRendering() const override;
};
