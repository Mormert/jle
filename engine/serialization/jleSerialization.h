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

#pragma once

#if JLE_BUILD_EDITOR
#include "modules/jleEditorModulesContext.h"
#endif

class jleResources;
class jleLuaEnvironment;
class jleRenderThread;

struct jleSerializationContext {
    jleSerializationContext() = default;

    jleSerializationContext(jleResources *r, jleLuaEnvironment *l, jleRenderThread *rt)
        : resources(r), luaEnvironment(l), renderThread(rt)
    {
    }

    // Optional, need to be null checked
    jleResources *resources{nullptr};

    // Optional, need to be null checked
    jleLuaEnvironment *luaEnvironment{nullptr};

    // Optional, need to be null checked
    jleRenderThread *renderThread{nullptr};
};

class jleSerializationArchive
{
public:
    explicit jleSerializationArchive(jleSerializationContext &context) : ctx{context} {}

    jleSerializationContext ctx;
};

#if JLE_BUILD_EDITOR
class jleSerializationArchive_EditorOnly : public jleSerializationArchive
{
public:
    jleSerializationArchive_EditorOnly(jleSerializationContext context, jleEditorModulesContext &editorContext)
        : jleSerializationArchive(context), editorCtx{editorContext}
    {
    }

    jleEditorModulesContext &editorCtx;
};
#endif