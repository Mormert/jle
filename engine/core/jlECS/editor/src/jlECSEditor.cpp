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

#include "jlECSEditor.h"

namespace jlECS
{

void
ComponentDebugBase::removeFromOwningObject(ObjectRef* objectRef)
{
    ecs->removeComponent(objectIndex, componentType);
    ObjectRef::gObjectRefDestructFunction(objectRef);
}


std::vector<ComponentDebugBase *> *
ObjectRef::componentsDebug()
{
    gObjectRefDestructFunction(this);
    auto *c = &ecs->objectArray.componentIndices[_objectIndex * ecs->registeredComponentTypesCount];
    for (int componentType = 0; componentType < ecs->registeredComponentTypesCount; componentType++) {
        if (c[componentType] != 65535) {
            auto* container = ecs->componentContainers[componentType].get();
            auto* editorContainer  = reinterpret_cast<ComponentContainerEditor*>(container);
            componentsDebug_.push_back(editorContainer->getComponentDebug(c[componentType], objectIndex(), ecs));
        }
    }
    return &componentsDebug_;
}

std::vector<std::unique_ptr<ComponentDebugBase>>
ObjectRef::componentsDebug2()
{
    std::vector<std::unique_ptr<ComponentDebugBase>> ret;
    auto *c = &ecs->objectArray.componentIndices[_objectIndex * ecs->registeredComponentTypesCount];
    for (int componentType = 0; componentType < ecs->registeredComponentTypesCount; componentType++) {
        if (c[componentType] != 65535) {
            auto* container = ecs->componentContainers[componentType].get();
            auto* editorContainer  = reinterpret_cast<ComponentContainerEditor*>(container);
            ret.push_back(std::unique_ptr<ComponentDebugBase>(editorContainer->getComponentDebug(c[componentType], objectIndex(), ecs)));
        }
    }

    return ret;
}

void
ECS_Debug::ObjectRefDestruct(ObjectRef *objectRef)
{
    for (auto *ptr : objectRef->componentsDebug_) {
        delete ptr;
    }
    objectRef->componentsDebug_.clear();
}

}
