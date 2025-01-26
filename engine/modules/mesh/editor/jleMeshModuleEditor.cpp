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

#include "jleMeshModuleEditor.h"

#include <jlECS/jlECS.h>
#include <editor/serialization/jleEditorECSRegistration.h>
#include <modules/hierarchy/components/cTransform.h>

#include "modules/mesh/components/cMesh.h"

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
jleMeshModuleEditor::initializeECS(jlECS::ECS &ecs)
{
    jlECS::ComponentRegistrationConfig config{
        .onCreateCallback = [this](jlECS::CreateComponentData& data) {
            onMeshComponentCreated(data);
        },
        .onDestroyCallback = [this](jlECS::DestroyComponentData& data) {
            onMeshComponentDestroyed(data);
        },
        .serializeInputF_JSON = jlECS::Serialization::serializeInputT_JSON<cMesh>,
        .serializeOutputF_JSON = jlECS::Serialization::serializeOutputT_JSON<cMesh>,
        .serializeInputF_Binary = jlECS::Serialization::serializeInputT_Binary<cMesh>,
        .serializeOutputF_Binary = jlECS::Serialization::serializeOutputT_Binary<cMesh>,
        .serializeImGuiF = serializeMeshEditor
    };
    ecs.registerComponentType<cMesh>(config);
}