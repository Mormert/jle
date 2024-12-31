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

#pragma once

#include <core/serialization/jleExternalSerialization.h>

class cParent{
public:
    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_parentIndex),
           CEREAL_NVP(_recycleCounter));
    }

    [[nodiscard]] uint16_t getParentIndex() const{ return _parentIndex; }
    [[nodiscard]] jlECS::ObjectRef getParentRef(jlECS::ECS& ecs) const { return jlECS::ObjectRef{_parentIndex, _recycleCounter, &ecs}; }

    static bool setParent(jlECS::ObjectRef& object, std::optional<jlECS::ObjectRef>& newParentOptional)
    {
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

private:
    uint16_t _parentIndex = 0;
    uint16_t _recycleCounter = std::numeric_limits<uint16_t>::max();
};