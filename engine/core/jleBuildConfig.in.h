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

// Build configuration file, generates by jleBuildConfig.in.h when CMake configure is run
// Use: #include "jleBuildConfig.h" and not #include "jleBuildConfig.in.h"

#pragma once

#cmakedefine01 JLE_BUILD_RUNTIME_CONFIGURABLE
#cmakedefine01 JLE_BUILD_HEADLESS
#cmakedefine01 JLE_BUILD_EDITOR
#cmakedefine01 JLE_BUILD_OPENGLES30

#if JLE_BUILD_EDITOR
#define JLE_BUILD_IMGUI 1
#else
#define JLE_BUILD_IMGUI 0
#endif

#if JLE_BUILD_RUNTIME_CONFIGURABLE
inline bool JLE_BUILD_HEADLESS_RUNTIME = JLE_BUILD_HEADLESS;
inline bool JLE_BUILD_EDITOR_RUNTIME = JLE_BUILD_EDITOR;
inline bool JLE_BUILD_OPENGLES30_RUNTIME = JLE_BUILD_OPENGLES30;
inline bool JLE_BUILD_IMGUI_RUNTIME = JLE_BUILD_IMGUI;
#endif

// JLE_EXEC_IF and JLE_EXEC_IF_NOT:
// A macro used to enable or disable functionality in the engine (run with editor, headless mode, etc.)
// and it can be used either at compile time using for example -DJLE_BUILD_EDITOR=ON or at runtime by
// providing command line arguments, like for example JLE_BUILD_EDITOR

// NOTE: in debug builds, the engine will use command line arguments only
// in non-debug, the engine will use comp-time defines only.

// Use case:
//
//    JLE_EXEC_IF(JLE_BUILD_EDITOR)
//    {
//          // Editor code.
//    }
//    else
//    {
//          // Non-editor code.
//    }

#if JLE_BUILD_RUNTIME_CONFIGURABLE
#define JLE_EXEC_IF(compileFlag) if (compileFlag##_RUNTIME)
#else
#define JLE_EXEC_IF(compileFlag) if constexpr (compileFlag)
#endif

#if JLE_BUILD_RUNTIME_CONFIGURABLE
#define JLE_EXEC_IF_NOT(compileFlag) if (!compileFlag##_RUNTIME)
#else
#define JLE_EXEC_IF_NOT(compileFlag) if constexpr (!(compileFlag))
#endif
