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

#include "jlECSSaveLoad.h"

#include <jlECS/jlECS.h>

#include "serialization/jleBinaryArchive.h"
#include "serialization/jleJSONArchive.h"

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace
{
template <class Archive>
void
processEcsObjectArray(Archive &archive, jlECS::EcsObjectArray &objectArray)
{
    archive(CEREAL_NVP(objectArray.aliveObjectsCount));
    archive(CEREAL_NVP(objectArray.aliveObjects));
    archive(CEREAL_NVP(objectArray.freeIndices));
    archive(CEREAL_NVP(objectArray.objectRecycleCounter));
}

void
reconstructComponentIndicesVector(jlECS::ECS &ecs)
{
    auto &objectArray = ecs.getObjectArray();

    const int allocatedObjects = objectArray.aliveObjects.size();

    objectArray.componentIndices.clear();
    objectArray.componentIndices.resize(allocatedObjects * ecs.getRegisteredComponentTypesCount());

    for (auto &i : objectArray.componentIndices) {
        i = 65535;
    }

    auto &containers = ecs.getComponentContainers();
    for (int i = 0; i < containers.size(); i++) {
        auto *container = containers[i].get();
        const auto &objectIndices = container->getObjectIndicesRef();

        for (int componentIndex = 0; componentIndex < objectIndices.size(); componentIndex++) {
            int objectIndex = objectIndices[componentIndex];

            auto *c = &objectArray.componentIndices[objectIndex * ecs.getRegisteredComponentTypesCount()];
            c[i] = componentIndex;
        }
    }
}

} // namespace

void
jlECS::save(jlECS::ECS &ecs, jleJSONOutputArchive &archive)
{
    auto &objectArray = ecs.getObjectArray();

    processEcsObjectArray(archive, objectArray);

    auto &componentContainers = ecs.getComponentContainers();

    archive.setNextName("components");
    archive.startNode();
    for (auto &container : componentContainers) {
        std::string componentTypeName = container->getComponentTypeName();
        archive.setNextName(container->getComponentTypeName());
        {
            archive.startNode();

            int componentCount = container->componentCount();
            archive(CEREAL_NVP(componentCount));

            const auto &objectIndices = container->getObjectIndicesRef();
            archive(CEREAL_NVP(objectIndices));

            for (int componentIndex = 0; componentIndex < componentCount; componentIndex++) {
                container->serializeOutput(archive, componentIndex);
            }
            archive.finishNode();
        }
    }
    archive.finishNode();
}

void
jlECS::load(jlECS::ECS &ecs, jleJSONInputArchive &archive)
{
    auto &objectArray = ecs.getObjectArray();

    processEcsObjectArray(archive, objectArray);

    auto &componentContainers = ecs.getComponentContainers();

    archive.setNextName("components");
    archive.startNode();
    for (auto &container : componentContainers) {
        std::string componentTypeName = container->getComponentTypeName();
        archive.setNextName(container->getComponentTypeName());

        archive.startNode();
        {
            int componentCount = 0;
            archive(CEREAL_NVP(componentCount));
            container->allocateComponents(componentCount);

            auto &objectIndices = container->getObjectIndicesRef();
            archive(objectIndices);

            for (int componentIndex = 0; componentIndex < componentCount; componentIndex++) {
                container->serializeInput(archive, componentIndex);
            }
        }
        archive.finishNode();
    }
    archive.finishNode();

    reconstructComponentIndicesVector(ecs);
}

void
jlECS::save(jlECS::ECS &ecs, jleBinaryOutputArchive &archive)
{
    auto &objectArray = ecs.getObjectArray();

    processEcsObjectArray(archive, objectArray);

    auto &componentContainers = ecs.getComponentContainers();

    for (auto &container : componentContainers) {
        int componentCount = container->componentCount();
        archive(componentCount);

        const auto &objectIndices = container->getObjectIndicesRef();
        archive(objectIndices);

        for (int componentIndex = 0; componentIndex < componentCount; componentIndex++) {
            container->serializeOutput(archive, componentIndex);
        }
    }
}

void
jlECS::load(jlECS::ECS &ecs, jleBinaryInputArchive &archive)
{
    auto &objectArray = ecs.getObjectArray();

    processEcsObjectArray(archive, objectArray);

    auto &componentContainers = ecs.getComponentContainers();

    for (auto &container : componentContainers) {
        int componentCount = 0;
        archive(componentCount);
        container->allocateComponents(componentCount);

        auto &objectIndices = container->getObjectIndicesRef();
        archive(objectIndices);

        for (int componentIndex = 0; componentIndex < componentCount; componentIndex++) {
            container->serializeInput(archive, componentIndex);
        }
    }

    reconstructComponentIndicesVector(ecs);
}
