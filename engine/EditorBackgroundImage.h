// Copyright (c) 2022. Johan Lind

#pragma once

#include "iTextureCreator.h"
#include "iRenderingFactory.h"
#include "iQuadRenderingInternal.h"

#include <memory>

namespace jle {
    class EditorBackgroundImage final {
    public:
        EditorBackgroundImage(const Image &image, iTextureCreator &tc, iRenderingFactory &rf);

        void Render(iQuadRenderingInternal &quadRenderer, unsigned int width, unsigned int height);

    private:
        std::shared_ptr<iTexture> background_texture;
        std::unique_ptr<iFramebuffer> background_framebuffer;
        std::unique_ptr<iFullscreenRendering> background_fullscreen_renderer;
        TexturedQuad background_quad;
    };
}