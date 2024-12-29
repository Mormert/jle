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

#include "editor/serialization/jleEditorECSRegistration.h"

#include "modules/core/components/cTransform.h"
#include "modules/core/components/cParent.h"

namespace {
void serializeTransformEditor(jlECS::ComponentContainer *thiz, jleImGuiArchive &archive, int componentIndex, int objectIndex)
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

void serializeParentEditor(jlECS::ComponentContainer *thiz, jleImGuiArchive &archive, int componentIndex, int objectIndex)
{
    cParent &parentComponent = *thiz->getPtr<cParent>(componentIndex);

    auto& ecs = thiz->getECS();
    auto parentRef = parentComponent.getParentRef(ecs);

    if(!parentRef.isValid()){
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        ImGui::Text("Parent ID (%d) is invalid (rec. cnt = %d)", parentRef.objectIndex(), parentRef.recycleCounter());
        ImGui::PopStyleColor();
    }

    uint16_t parentIndex = parentComponent.getParentIndex();
    const uint16_t min = 0;
    const uint16_t max = ecs.aliveObjectsCount() - 1;
    ImGui::DragScalar(jleImGuiHelpers::LeftLabelImGui("Parent").c_str(), ImGuiDataType_U16, &parentIndex, 0.2f, &min, &max, "%u");
    if(parentIndex != parentComponent.getParentIndex() && ecs.isObjectAlive(parentIndex)){
        auto newParentRef = ecs.getObject(parentIndex);
        parentComponent.setParent(newParentRef);
    }

    if(parentIndex == objectIndex){
        ImGui::Text("Parent ID (%d) can't be same as object ID!", parentIndex);
        return;
    }
}

}

void
jleCoreModuleEditor::initializeECS(jlECS::ECS &ecs)
{
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

    {
        jlECS::ComponentRegistrationConfig config{
            .serializeInputF_JSON = jlECS::Serialization::serializeInputT_JSON<cParent>,
            .serializeOutputF_JSON = jlECS::Serialization::serializeOutputT_JSON<cParent>,
            .serializeInputF_Binary = jlECS::Serialization::serializeInputT_Binary<cParent>,
            .serializeOutputF_Binary = jlECS::Serialization::serializeOutputT_Binary<cParent>,
            .serializeImGuiF = serializeParentEditor
        };

        ecs.registerComponentType<cParent>(config);
    }
}
