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

#include "jlePhysicsModuleEditor.h"

#include <editor/serialization/jleEditorECSRegistration.h>

#include "modules/physics/components/cRigidbody.h"

void
jlePhysicsModuleEditor::initializeECS(jlECS::ECS &ecs)
{
    {
        jlECS::ComponentRegistrationConfig config;
        setupDefaultEditorComponentConfig<cRigidbody>(config);
        config.onCreateCallback = [this](jlECS::CreateCallbackData& data) {
            onRigidbodyCreated(data);
        };
        config.onDestroyCallback = [this](void* componentPtr) {
            onRigidbodyDestroyed(static_cast<cRigidbody*>(componentPtr));
        };
        config.onDuplicateCallback = [this](void* sourceComp, void* destComp) {
            onRigidbodyCopied(static_cast<cRigidbody*>(sourceComp), static_cast<cRigidbody*>(destComp));
        };

        ecs.registerComponentType<cRigidbody>(config);
    }
}
