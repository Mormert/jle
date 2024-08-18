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
class jle3DRenderer;
class jleFrameInfo;

struct jleEngineModulesContext {
    explicit jleEngineModulesContext(
        jle3DRenderer &renderer, jleInput &input, jleWindow &window, jleResources &resources, jleFrameInfo &info);

    // Modules
    jle3DRenderer &rendererModule;
    jleWindow &windowModule;
    jleResources &resourcesModule;
    jleInput &inputModule;

    // Utilities
    jleFrameInfo &frameInfo;
};