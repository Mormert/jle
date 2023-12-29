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

#include "jleNew.h"
#include <Tracy.hpp>

void *
operator new[](size_t size, const char *pName, int flags, unsigned int debugFlags, const char *file, int line)
{
    void *ptr = malloc(size);
    TracyAlloc(ptr, size);
    return ptr;
}

void *
operator new[](size_t size,
               size_t alignment,
               size_t alignmentOffset,
               const char *pName,
               int flags,
               unsigned int debugFlags,
               const char *file,
               int line)
{
    void *ptr = malloc(size);
    TracyAlloc(ptr, size);
    return ptr;
}

void *
operator new(std::size_t n)
{
    void *ptr = malloc(n);
    TracyAlloc(ptr, n);
    return ptr;
}

void
operator delete(void *ptr) noexcept
{
    TracyFree(ptr);
    free(ptr);
}
