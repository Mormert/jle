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

#include "jleHierarchyModule.h"

#include <jlECS/jlECS.h>
#include <core/serialization/jleJSONArchive.h>
#include <core/serialization/jleBinaryArchive.h>

#include "components/cTransform.h"
#include "components/cParent.h"

namespace {
    // Recursively compute and set each object's world transform in-place
    void computeWorldMatrixRecursive(const jlECS::ObjectRef &object,
                                     jlECS::ECS &ecs,
                                     std::vector<glm::mat4> &worldMatrices,
                                     std::vector<bool> &touchedTransforms)
    {
        const auto index = object.objectIndex();

        // If we've already computed this object's world transform, just return
        if (touchedTransforms[index]) {
            return;
        }

        const auto* transformComp = object.getComponentPtr<cTransform>();
        const glm::mat4 localMatrix = transformComp ? transformComp->getLocalMatrix() : glm::mat4(1.0f);

        const auto* parentComp = object.getComponentPtr<cParent>();
        if (!parentComp)
        {
            worldMatrices[index] = localMatrix;
            touchedTransforms[index] = true;
            return;
        }

        auto parentObj = parentComp->getParentRef(ecs);
        if (!touchedTransforms[parentObj.objectIndex()]) {
            computeWorldMatrixRecursive(parentObj, ecs, worldMatrices, touchedTransforms);
        }

        const glm::mat4 parentWorldMatrix = worldMatrices[parentObj.objectIndex()];
        const glm::mat4 finalWorldMatrix = parentWorldMatrix * localMatrix;

        worldMatrices[index] = finalWorldMatrix;
        touchedTransforms[index] = true;
    }


}

void
jleHierarchyModule::initializeECS(jlECS::ECS &ecs)
{
    ecs.registerComponentType<cTransform>();
    ecs.registerComponentType<cParent>();
}

void jleHierarchyModule::updateWorldMatrices(jlECS::ECS &ecs) {
    _worldTransforms.resize(ecs.allocatedObjectsCount());
    _touchedTransforms.assign(ecs.allocatedObjectsCount(), false);

    for (auto [objectIndex, transform] : ecs.iterateMulti_IncludeObjectIndex<cTransform>())
    {
        jlECS::ObjectRef object = ecs.getObject(objectIndex);
        computeWorldMatrixRecursive(object, ecs, _worldTransforms, _touchedTransforms);
    }
}
