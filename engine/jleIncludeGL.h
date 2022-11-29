// Copyright (c) 2022. Johan Lind

#ifndef JLE_INCLUDE_GL
#define JLE_INCLUDE_GL

// When building with Emscripten, grab other include files
// The extensions found in gl2ext are also found in glad.h
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <emscripten.h>

#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else
#include <glad/glad.h>
#endif

#endif // JLE_INCLUDE_GL