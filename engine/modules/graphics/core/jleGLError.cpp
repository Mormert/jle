/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleGLError.h"

#ifndef NDEBUG
GLenum
glCheckError_(const char *desc, const char *file, int line)
{
    {
        GLenum errorCode;
        while ((errorCode = glGetError()) != 0)
        {
            std::string error;
            switch (errorCode)
            {
            case 0x0500:    error = "INVALID_ENUM"; break;
            case 0x0501:    error = "INVALID_VALUE"; break;
            case 0x0502:    error = "INVALID_OPERATION"; break;
            case 0x0503:    error = "STACK_OVERFLOW"; break;
            case 0x0504:    error = "STACK_UNDERFLOW"; break;
            case 0x0505:    error = "OUT_OF_MEMORY"; break;
            case 0x0506:    error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            LOGE << "'" << desc << "' - " << error << " | " << file << " (" << line << ")";
        }
        return errorCode;
    }
}
#endif