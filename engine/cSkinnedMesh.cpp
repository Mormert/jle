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
#include "cAnimator.h"
#include "jleAnimationFinalMatrices.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cSkinnedMesh)

cSkinnedMesh::cSkinnedMesh(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cSkinnedMesh::editorUpdate(float dt)
{
    if (_animator && _animator->isDestroyed()) {
        _animator.reset();
    }

    findAnimator(object());
    update(dt);
}

void
cSkinnedMesh::start()
{
    findAnimator(object());
}

void
cSkinnedMesh::update(float dt)
{
    std::shared_ptr<jleAnimationFinalMatrices> animationMatrices;
    if (_animator) {
        animationMatrices = _animator->animationMatrices();
    } else {
        static std::shared_ptr<jleAnimationFinalMatrices> identityMatrices =
            std::make_shared<jleAnimationFinalMatrices>();
        animationMatrices = identityMatrices;
    }

    if (_skinnedMeshRef) {
        std::shared_ptr<jleSkinnedMesh> mesh = _skinnedMeshRef.get();
        std::shared_ptr<jleMaterial> material = _materialRef.get();
        gEngine->renderGraph().sendSkinnedMesh(
            mesh, material, animationMatrices, getTransform().getWorldMatrix(), _attachedToObject->instanceID(), true);
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

void
cSkinnedMesh::findAnimator(jleObject *object)
{
    if (auto animator = object->getComponent<cAnimator>()) {
        _animator = animator;
        return;
    } else {
        if (auto parent = object->parent()) {
            findAnimator(parent);
            return;
        }
    }
}

void
cSkinnedMesh::editorInspectorImGuiRender()
{
#ifdef BUILD_EDITOR
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

template <class Archive>
void
cSkinnedMesh::serialize(Archive &ar)
{
    ar(CEREAL_NVP(_skinnedMeshRef), CEREAL_NVP(_materialRef));
}
