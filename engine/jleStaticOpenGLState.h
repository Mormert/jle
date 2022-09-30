// Copyright (c) 2022. Johan Lind

#pragma once

// Contains global data corresponding to the OpenGL State
struct jleStaticOpenGLState {
    static bool globalOpenGLInitialized;
    static unsigned int globalActiveTexture;
};
