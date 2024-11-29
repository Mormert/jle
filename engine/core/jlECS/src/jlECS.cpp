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
ObjectRef::componentCount()
{
    uint16_t count = 0;
    auto *c = &ecs->objectArray.componentIndices[_objectIndex * ecs->registeredComponentTypesCount];
    for (int componentType = 0; componentType < ecs->registeredComponentTypesCount; componentType++) {
        if (c[componentType] != 65535) {
            count++;
        }
    }
    return count;
}

#if JLECS_USE_DEBUG

std::vector<ComponentDebugBase *> *
ObjectRef::componentsDebug()
{
    clearComponentsDebug();
    auto *c = &ecs->objectArray.componentIndices[_objectIndex * ecs->registeredComponentTypesCount];
    for (int componentType = 0; componentType < ecs->registeredComponentTypesCount; componentType++) {
        if (c[componentType] != 65535) {
            componentsDebug_.push_back(
                ecs->componentContainers[componentType].getComponentDebug(c[componentType], objectIndex(), ecs));
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
            ret.push_back(std::unique_ptr<ComponentDebugBase>(
                ecs->componentContainers[componentType].getComponentDebug(c[componentType], objectIndex(), ecs)));
        }
    }

    return ret;
}

ObjectRef::~ObjectRef() { clearComponentsDebug(); }

void
ObjectRef::clearComponentsDebug()
{
    for (auto *ptr : componentsDebug_) {
        delete ptr;
    }
    componentsDebug_.clear();
}


void
ComponentDebugBase::removeFromOwningObject()
{
    ecs->removeComponent(objectIndex, componentType);
}

#endif

} // namespace jlECS
