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

#include <memory>
#include <vector>

class jleWindow;
class jleResources;
class jleInput;
class jleLuaEnvironment;
class jle3DRenderer;
struct jle3DSettings;
class jle3DGraph;
class jleFrameInfo;

struct jleEngineModulesContext {
    explicit jleEngineModulesContext(jle3DRenderer &renderer,
                                     jle3DSettings &renderSettings,
                                     jle3DGraph &renderGraph,
                                     jleInput &input,
                                     jleLuaEnvironment& luaEnvironment,
                                     jleWindow &window,
                                     jleResources &resources,
                                     jleFrameInfo &info);

    // Modules
    jle3DRenderer &rendererModule;
    jleWindow &windowModule;
    jleResources &resourcesModule;
    jleInput &inputModule;
    jleLuaEnvironment& luaEnvironment;

    // Rendering
    jle3DSettings &renderSettings;
    jle3DGraph &renderGraph;

    // Utilities
    jleFrameInfo &frameInfo;
};