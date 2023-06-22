// Copyright (c) 2023. Johan Lind

#ifndef JLE_INCLUDE_GL
#define JLE_INCLUDE_GL

#include <plog/Log.h>

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

static inline GLenum glCheckError_(const char* desc, const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        LOGE << "'" << desc << "' - " << error << " | " << file << " (" << line << ")";
    }
    return errorCode;
}
#ifndef NDEBUG
#define glCheckError(desc) glCheckError_(desc, __FILE__, __LINE__)
#else
#define glCheckError()
#endif

#endif // JLE_INCLUDE_GL