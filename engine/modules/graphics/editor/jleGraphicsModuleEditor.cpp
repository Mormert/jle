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

#include "jleGraphicsModuleEditor.h"

#include <editor/serialization/jleEditorECSRegistration.h>

#include "modules/graphics/runtime/components/cCamera.h"
#include "modules/graphics/runtime/components/cLight.h"
#include "modules/graphics/runtime/components/cLightDirectional.h"
#include "modules/graphics/runtime/components/cMesh.h"
#include "modules/graphics/runtime/components/cSkinnedMesh.h"
#include "modules/graphics/runtime/components/cSkybox.h"


namespace {
    void serializeMeshEditor(jlECS::ComponentContainer *thiz, jleImGuiArchive &archive, int componentIndex, int objectIndex)
    {
        cMesh &meshComponent = *thiz->getPtr<cMesh>(componentIndex);
        archive(meshComponent);

        auto& ecs = thiz->getECS();
        if (auto object = ecs.getObject(objectIndex); !object.getComponentPtr<cTransform>()) {
            ImGui::Text("Object does not have a transform!");
            if (ImGui::Button("Add cTransform")) {
                object.addComponent<cTransform>();
            }
        }
    }
}

void
jleGraphicsModuleEditor::initializeECS(jlECS::ECS &ecs)
{
    registerEditorECSComponent<cCamera>(ecs);
    registerEditorECSComponent<cLight>(ecs);
    registerEditorECSComponent<cLightDirectional>(ecs);

    {
        jlECS::ComponentRegistrationConfig config{
            .serializeInputF_JSON = jlECS::Serialization::serializeInputT_JSON<cMesh>,
            .serializeOutputF_JSON = jlECS::Serialization::serializeOutputT_JSON<cMesh>,
            .serializeInputF_Binary = jlECS::Serialization::serializeInputT_Binary<cMesh>,
            .serializeOutputF_Binary = jlECS::Serialization::serializeOutputT_Binary<cMesh>,
            .serializeImGuiF = serializeMeshEditor
        };
        ecs.registerComponentType<cMesh>(config);
    }


    registerEditorECSComponent<cSkinnedMesh>(ecs);
    registerEditorECSComponent<cSkybox>(ecs);
}
