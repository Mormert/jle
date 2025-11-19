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

#include "jleHierarchyFuncs.h"

#include "components/cParent.h"
#include "components/cTransform.h"
#include "jlECS/jlECS.h"

namespace jleHierarchyFuncs{
    glm::mat4 getWorldMatrix(const jlECS::ObjectRef &object)
    {
        glm::mat4 worldMatrix{1.0f};
        jlECS::ObjectRef current = object;

        while (current.isValid())
        {
            const auto* transformComp = current.getComponentPtr<cTransform>();
            if (transformComp) {
                worldMatrix = transformComp->getLocalMatrix() * worldMatrix;
            }

            // Move to the parent
            const auto* parentComp = current.getComponentPtr<cParent>();
            if (!parentComp)
            {
                break;
            }

            jlECS::ObjectRef parentRef = parentComp->getParentRef(object.getECS());
            if (!parentRef.isValid()) {
                break;
            }

            current = parentRef;
        }

        return worldMatrix;
    }

    glm::vec3 getWorldPosition(const jlECS::ObjectRef &object) {
        return glm::vec3(getWorldMatrix(object)[3]);
    }

    void setLocalMatrixFromWorld(const jlECS::ObjectRef &object, const glm::mat4 &worldMatrix) {
        auto* transformComp = object.getComponentPtr<cTransform>();
        if (!transformComp) {
            return;
        }

        glm::mat4 parentWorld(1.0f);

        if (auto* parentComp = object.getComponentPtr<cParent>()) {
            jlECS::ObjectRef parentRef = parentComp->getParentRef(object.getECS());
            if (parentRef.isValid()) {
                parentWorld = getWorldMatrix(parentRef);
            }
        }

        glm::mat4 localMatrix = glm::inverse(parentWorld) * worldMatrix;

        transformComp->setLocalMatrix(localMatrix);
    }

    void destroyObjectIncludeChildren(const jlECS::ObjectRef &objectToDestroy) {
        assert(objectToDestroy.isValid());

        auto& ecs = objectToDestroy.getECS();

        const int allocatedObjects = ecs.getObjectArray().aliveObjects.size();
        for (int i = 0; i < allocatedObjects; i++) {
            if (!ecs.isObjectAlive(i)) continue;
            auto potentialChild = ecs.getObject(i);

            auto* parentComponent = ecs.getComponent<cParent>(potentialChild.objectIndex());
            if (parentComponent && parentComponent->getParentRef(ecs) == objectToDestroy) {
                destroyObjectIncludeChildren(potentialChild);
            }
        }

        ecs.destroyObject(objectToDestroy);
    }

    jlECS::ObjectRef duplicateObjectIncludeChildren(const jlECS::ObjectRef &originalObject,
        std::vector<jlECS::ObjectRef> &newObjects) {
        assert(originalObject.isValid());

        auto& ecs = originalObject.getECS();
        jlECS::ObjectRef newObject = originalObject.getECS().duplicateObject(originalObject);

        const int allocatedObjects = ecs.getObjectArray().aliveObjects.size();
        for (int i = 0; i < allocatedObjects; i++) {
            if (!ecs.isObjectAlive(i)) continue;
            auto potentialChild = ecs.getObject(i);

            auto* parentComponent = ecs.getComponent<cParent>(potentialChild.objectIndex());
            if (parentComponent && parentComponent->getParentRef(ecs) == originalObject) {
                jlECS::ObjectRef duplicatedChild = duplicateObjectIncludeChildren(potentialChild, newObjects);

                std::optional<jlECS::ObjectRef> newObjectOptional(newObject);
                setParent(duplicatedChild, newObjectOptional);
            }
        }

        newObjects.emplace_back(std::move(newObject));
        return newObject;
    }

    bool setParent(jlECS::ObjectRef &object, std::optional<jlECS::ObjectRef> &newParentOptional) {
        if (!newParentOptional.has_value()) { // setting the child to be a root-level object
            if (object.getComponentPtr<cParent>()) {
                object.removeComponent<cParent>();
            }
            return true;
        }

        jlECS::ObjectRef& newParent = newParentOptional.value();
        if (newParent == object)
        {
            return false;
        }

        assert(&newParent.getECS() == &object.getECS());
        auto& ecs = newParent.getECS();

        bool objectAlreadyHadParentComponent = true;
        cParent* parentComponent = object.getComponentPtr<cParent>();
        cParent parentComponentTemp;
        if (!parentComponent) {
            parentComponent = object.addComponent<cParent>().get();
            objectAlreadyHadParentComponent = false;
        }else {
            parentComponentTemp = *parentComponent;
        }

        bool removeParentComponentFromParent = false;
        cParent* currentParentComponent = newParent.getComponentPtr<cParent>();
        int iterations = 0;
        while (currentParentComponent) {
            jlECS::ObjectRef currentParentObject = currentParentComponent->getParentRef(ecs);

            // If the current parent object is the child, update and break
            if (iterations == 0 && currentParentObject == object) {
                parentComponent->_parentIndex = newParent.objectIndex();
                parentComponent->_recycleCounter = newParent.recycleCounter();
                removeParentComponentFromParent = true;
                break;
            }

            // Move to the next parent in the chain
            currentParentComponent = currentParentObject.getComponentPtr<cParent>();
            iterations++;
        }

        if (removeParentComponentFromParent) {
            if (!objectAlreadyHadParentComponent) {
                newParent.removeComponent<cParent>();
            }else {
                parentComponent->_parentIndex    = newParent.objectIndex();
                parentComponent->_recycleCounter = newParent.recycleCounter();
                currentParentComponent->_parentIndex = parentComponentTemp._parentIndex;
                currentParentComponent->_recycleCounter = parentComponentTemp._recycleCounter;
            }
        }else {
            parentComponent->_parentIndex    = newParent.objectIndex();
            parentComponent->_recycleCounter = newParent.recycleCounter();
        }

        return true;
    }
}
