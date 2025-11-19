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

#include <modules/jleEditorUpdateContext.h>
#include "modules/physics/jlePhysicsModule.h"

class jlePhysicsModuleEditor : public jlePhysicsModule
{
public:
    void initializeECS(jlECS::ECS &ecs) override;

    void updateEditor(jleFramePacket &framePacket);

    bool& getDebugRenderingEnabledRef(){ return _renderPhysicsDebug; }

private:
    bool _renderPhysicsDebug = false;
};


