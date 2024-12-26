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

#include "jleCoreModuleEditor.h"
#include "jlECS/jlECS.h"

#include <ImGui/ImGuizmo.h>
#include <ImGui/imgui.h>

#include "core/serialization/jleBinaryArchive.h"
#include "core/serialization/jleJSONArchive.h"

#include "modules/core/components/cTransform.h"

namespace {
void serializeTransformEditor(jlECS::ComponentContainer *thiz, jleImGuiArchive &archive, int componentIndex)
{
    cTransform &transform = *thiz->getPtr<cTransform>(componentIndex);

    glm::mat4 cpyMat = transform.getWorldMatrix();
    float* cpyMatPtr = &cpyMat[0][0];

    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(cpyMatPtr, matrixTranslation, matrixRotation, matrixScale);

    bool changed = false;

    ImGui::Text("Mat4x4");

    changed |= ImGui::DragFloat3("Translation", matrixTranslation, 0.1f);
    changed |= ImGui::DragFloat3("Rotation",    matrixRotation,    0.1f);
    changed |= ImGui::DragFloat3("Scaling",     matrixScale,       0.1f);

    if (changed)
    {
        ImGuizmo::RecomposeMatrixFromComponents(
            matrixTranslation,
            matrixRotation,
            matrixScale,
            cpyMatPtr
        );
        transform.setWorldMatrix(cpyMat);
    }
}
}

void
jleCoreModuleEditor::initializeECS(jlECS::ECS &ecs)
{
    jlECS::ComponentRegistrationConfig config{
        .serializeInputF_JSON = jlECS::Serialization::serializeInputT_JSON<cTransform>,
        .serializeOutputF_JSON = jlECS::Serialization::serializeOutputT_JSON<cTransform>,
        .serializeInputF_Binary = jlECS::Serialization::serializeInputT_Binary<cTransform>,
        .serializeOutputF_Binary = jlECS::Serialization::serializeOutputT_Binary<cTransform>,
        .serializeImGuiF = serializeTransformEditor
    };

    ecs.registerComponentType<cTransform>(config);
}
