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
*                  Copyright (c) 2020-2025 Johan Lind. All rights reserved.                 *
*                                                                                           *
*********************************************************************************************/

#pragma once

#include <vector>
#include <optional>
#include <glm/glm.hpp>

namespace jlECS{
class ObjectRef;
}

namespace jleHierarchyFuncs {
    // Will traverse all the parents in the chain and calculate the world matrix (expensive if deep in the hierarchy)
    glm::mat4 getWorldMatrix(const jlECS::ObjectRef& object);

    glm::vec3 getWorldPosition(const jlECS::ObjectRef& object);

    void setLocalMatrixFromWorld(const jlECS::ObjectRef& object, const glm::mat4& worldMatrix);

    // Expensive operation as it needs to iterate through all objects in the ECS to find the children!
    void destroyObjectIncludeChildren(const jlECS::ObjectRef &objectToDestroy);

    // Expensive operation as it needs to iterate through all objects in the ECS to find the children!
    jlECS::ObjectRef duplicateObjectIncludeChildren(const jlECS::ObjectRef &originalObject, std::vector<jlECS::ObjectRef>& newObjects);

    bool setParent(jlECS::ObjectRef& object, std::optional<jlECS::ObjectRef>& newParentOptional);
};

