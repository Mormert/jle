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

#include "jlECS.h"

namespace jlECS
{

ObjectRef::ObjectRef(uint16_t objectIndex, uint16_t objectRecycleCounter, ECS *ecs)
    : _objectIndex{objectIndex}, _objectRecycleCounter{objectRecycleCounter}, ecs{ecs}
{
}

uint16_t
ObjectRef::componentCount() const {
    uint16_t count = 0;
    auto *c = &ecs->objectArray.componentIndices[_objectIndex * ecs->registeredComponentTypesCount];
    for (int componentType = 0; componentType < ecs->registeredComponentTypesCount; componentType++) {
        if (c[componentType] != 65535) {
            count++;
        }
    }
    return count;
}

ObjectRef::~ObjectRef() {
    if(gObjectRefDestructFunction)
        gObjectRefDestructFunction(this);
}

void
ComponentContainer::callAddComponentConstruct(ComponentContainer *thiz, uint16_t objectIndex, uint16_t componentIndex, void* component)
{
    CreateCallbackData data{ObjectRef{thiz->ecs->getObject(objectIndex)}, componentIndex, component};
    thiz->onCreateCallback(data);
}

std::vector<Debug::ComponentDebugBase *> *
ObjectRef::componentsDebug()
{
    gObjectRefDestructFunction(this);
    auto *c = &ecs->objectArray.componentIndices[_objectIndex * ecs->registeredComponentTypesCount];
    for (int componentType = 0; componentType < ecs->registeredComponentTypesCount; componentType++) {
        if (c[componentType] != 65535) {
            auto* container = ecs->componentContainers[componentType].get();
            auto* editorContainer  = reinterpret_cast<Debug::ComponentContainerEditor*>(container);
            componentsDebug_.push_back(editorContainer->getComponentDebug(c[componentType], objectIndex(), ecs));
        }
    }
    return &componentsDebug_;
}

std::vector<std::unique_ptr<Debug::ComponentDebugBase>>
ObjectRef::componentsDebug2()
{
    std::vector<std::unique_ptr<Debug::ComponentDebugBase>> ret;
    auto *c = &ecs->objectArray.componentIndices[_objectIndex * ecs->registeredComponentTypesCount];
    for (int componentType = 0; componentType < ecs->registeredComponentTypesCount; componentType++) {
        if (c[componentType] != 65535) {
            auto* container = ecs->componentContainers[componentType].get();
            auto* editorContainer  = reinterpret_cast<Debug::ComponentContainerEditor*>(container);
            ret.push_back(std::unique_ptr<Debug::ComponentDebugBase>(editorContainer->getComponentDebug(c[componentType], objectIndex(), ecs)));
        }
    }

    return ret;
}

namespace Debug{

void
ComponentDebugBase::removeFromOwningObject(ObjectRef* objectRef)
{
    ecs->removeComponent(objectIndex, componentType);
    ObjectRef::gObjectRefDestructFunction(objectRef);
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

} // namespace jlECS
