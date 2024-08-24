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

class jleResources;
class jleLuaEnvironment;

struct jleSerializationContext {
    jleSerializationContext() = default;

    jleSerializationContext(jleResources *r, jleLuaEnvironment *l) : resources(r), luaEnvironment(l) {}

    // Optional, need to be null checked
    jleResources *resources{nullptr};

    // Optional, need to be null checked
    jleLuaEnvironment *luaEnvironment{nullptr};
};

class jleSerializationArchive
{
public:
    explicit jleSerializationArchive(jleSerializationContext &context) : ctx{context} {}

    jleSerializationContext ctx;
};
