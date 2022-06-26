// Copyright (c) 2022. Johan Lind

#pragma once

namespace jle {
    // Contains global data corresponding to the OpenGL State
    struct jleStaticOpenGLState {
        static bool globalOpenGLInitialized;
        static unsigned int globalActiveTexture;
    };
}
