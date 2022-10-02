// Copyright (c) 2022. Johan Lind

#pragma once

class jleFramebuffer_OpenGL;

class jleFullscreenRenderingInterface {
public:
    virtual ~jleFullscreenRenderingInterface() = default;

    virtual void RenderFramebufferFullscreen(jleFramebuffer_OpenGL& framebuffer,
                                             unsigned int screenWidth,
                                             unsigned int screenHeight) = 0;
};
