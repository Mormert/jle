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

#ifndef JLE_ASSERT_H
#define JLE_ASSERT_H

#ifndef NDEBUG

#include <plog/Log.h>

#ifdef _WIN32
#include <intrin.h>
#define JLE_DEBUG_BREAK __debugbreak()
#else
#include <signal.h>
#define JLE_DEBUG_BREAK raise(SIGTRAP)
#endif

// Will only assert once, which can be convenient sometimes to avoid assertion spam
#define jleAssertOnce(condition)                                                                                       \
    do {                                                                                                               \
        static bool triggeredOnce{false};                                                                              \
        if (!(condition) && !triggeredOnce) {                                                                          \
            triggeredOnce = true;                                                                                      \
            LOGE << "Assertion failed: " << #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl;          \
            JLE_DEBUG_BREAK;                                                                                           \
        }                                                                                                              \
    } while (0)

#define jleAssert(condition)                                                                                           \
    do {                                                                                                               \
        if (!(condition)) {                                                                                            \
            LOGE << "Assertion failed: " << #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl;          \
            JLE_DEBUG_BREAK;                                                                                           \
        }                                                                                                              \
    } while (0)

#else
#define jleAssert(condition) ((void)0)
#define jleAssertOnce(condition) ((void)0)
#endif

#endif // JLE_ASSERT_H
