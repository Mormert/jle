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

#include "cSkinnedMesh.h"
#include "modules/core/components/cTransform.h"
#include "modules/animation/components/cAnimator.h"
#include "modules/animation/jleAnimationFinalMatrices.h"

void
cSkinnedMesh::ecsUpdate(jleFramePacket &packet, const cTransform& transform, const cAnimator* optionalAnimator, int objectIndex)
{
    std::shared_ptr<jleAnimationFinalMatrices> animationMatrices;
    if (optionalAnimator) {
        animationMatrices = optionalAnimator->animationMatrices();
    } else {
        static std::shared_ptr<jleAnimationFinalMatrices> identityMatrices = std::make_shared<jleAnimationFinalMatrices>();
        animationMatrices = identityMatrices;
    }

    if (_skinnedMeshRef) {
        std::shared_ptr<jleSkinnedMesh> mesh = _skinnedMeshRef.get();
        std::shared_ptr<jleMaterial> material = _materialRef.get();
        packet.sendSkinnedMesh(mesh, material, animationMatrices, transform.getWorldMatrix(), objectIndex, true);
    }
}


std::shared_ptr<jleSkinnedMesh>
cSkinnedMesh::getMesh()
{
    return _skinnedMeshRef.get();
}

std::shared_ptr<jleMaterial>
cSkinnedMesh::getMaterial()
{
    return _materialRef.get();
}

jleResourceRef<jleSkinnedMesh> &
cSkinnedMesh::getMeshRef()
{
    return _skinnedMeshRef;
}

jleResourceRef<jleMaterial> &
cSkinnedMesh::getMaterialRef()
{
    return _materialRef;
}

/*
void
cSkinnedMesh::editorInspectorImGuiRender(jleEditorUpdateContext & ctx)
{
#if JLE_BUILD_IMGUI
    if (!_animator) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
        ImGui::Text(
            "Missing cAnimator for this skinned mesh!\nPlace a cAnimator component on this object, or one of its "
            "parents in the chain.");
        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
        ImGui::Text("Using cAnimator from: %s", _animator->object()->instanceName().c_str());
        ImGui::PopStyleColor();
    }
#endif

}
*/
