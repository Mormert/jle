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

#include <core/serialization/jleSerialization.h>

class jleWindow;
class jleResourceHolder;
class jleInput;
class jleLuaEnvironment;
class jleGraphics;
struct jle3DSettings;
class jleFramePacket;
class jleFrameInfo;
class jleGameRuntime;
class jleEngineSettings;
class jleRenderThread;

struct jleEngineUpdateContext {
    explicit jleEngineUpdateContext(jleGameRuntime &gameRuntime,
                                     jleGraphics &renderer,
                                     jleRenderThread &renderThread,
                                     jleFramePacket &renderGraph,
                                     jleEngineSettings &engineSettings,
                                     jleInput &input,
                                     jleWindow &window,
                                     jleResourceHolder &resources,
                                     jleFrameInfo &info,
                                     const jleSerializationContext & serializationContext);
    jleSerializationContext serializationContext;
    jleGameRuntime &gameRuntime;
    jleGraphics &graphics;
    jleWindow &window;
    jleResourceHolder &resourcesHolder;
    jleInput &input;
    jleRenderThread &renderThread;
    jleFramePacket &currentFramePacket;
    jleFrameInfo &frameInfo;
    jleEngineSettings &settings;
};