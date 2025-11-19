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

#include <core/serialization/jleSerialization.h>

class jleResourceHolder;
class jleFrameInfo;
class jleGameRuntime;
class jleEngineSettings;

struct jleEngineUpdateContext {
    explicit jleEngineUpdateContext(jleGameRuntime &gameRuntime,
                                     jleEngineSettings &engineSettings,
                                     jleResourceHolder &resources,
                                     jleFrameInfo &info,
                                     const jleSerializationContext & serializationContext);
    jleSerializationContext serializationContext;
    jleGameRuntime &gameRuntime;
    jleResourceHolder &resourcesHolder;
    jleFrameInfo &frameInfo;
    jleEngineSettings &settings;
};