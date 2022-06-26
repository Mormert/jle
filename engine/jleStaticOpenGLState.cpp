// Copyright (c) 2022. Johan Lind

#include "jleStaticOpenGLState.h"

namespace jle {
    bool jleStaticOpenGLState::globalOpenGLInitialized{false};
    unsigned int jleStaticOpenGLState::globalActiveTexture{0};
}