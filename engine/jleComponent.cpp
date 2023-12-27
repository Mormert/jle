// Copyright (c) 2023. Johan Lind

#include "jleComponent.h"
#include "jleObject.h"

#include "jleNetworkEvent.h"
#include "jleSceneClient.h"
#include "jleSceneServer.h"

jleComponent::jleComponent(jleObject *owner, jleScene *scene) : _attachedToObject{owner}, _containedInScene{scene} {}

void
jleComponent::destroy()
{
    _attachedToObject->destroyComponent(this);
}

jleTransform &
jleComponent::getTransform()
{
    return _attachedToObject->getTransform();
}

jleObject *
jleComponent::object()
{
    return _attachedToObject;
}

jleScene *
jleComponent::scene()
{
    return _containedInScene;
}

bool
jleComponent::isDestroyed()
{
    return _isDestroyed;
}

int
jleComponent::parallelUpdateBatchSize()
{
    return _parallelUpdateBatchSize;
}

bool
jleComponent::parallelUpdateEnabled()
{
    return _enableParallelUpdate;
}

void
jleComponent::enableParallelUpdate(int batchSize)
{
    _enableParallelUpdate = true;
    _parallelUpdateBatchSize = batchSize;
}

struct jleComponentNetSyncEvent : public jleServerToClientEvent {
    void
    execute() override
    {
        auto &scene = getSceneClient();
        if (auto object = scene.getObjectPointerFromNetEntity(netEntityId)) {
            auto component = object->customComponents()[componentIndex];
            try {
                std::stringstream stream{};
                stream.write(&serializedBinaryData[0], serializedBinaryData.size());
                cereal::BinaryInputArchive archive(stream);
                component->netSyncIn(archive);
            } catch (std::exception &e) {
                LOGE << "Failed parsing component net sync event: " << e.what();
            }
        }
    }

    template <class Archive>
    void
    serialize(Archive &archive)
    {
        archive(CEREAL_NVP(netEntityId), CEREAL_NVP(componentIndex), CEREAL_NVP(serializedBinaryData));
    }

    int32_t netEntityId;
    uint8_t componentIndex;
    std::vector<char> serializedBinaryData;
};

JLE_REGISTER_NET_EVENT(jleComponentNetSyncEvent)

void
jleComponent::syncServerToClient()
{
    auto event = jleMakeNetEvent<jleComponentNetSyncEvent>();

    std::stringstream componentStream{};
    {
        cereal::BinaryOutputArchive componentArchive(componentStream);
        netSyncOut(componentArchive);
    }

    const auto &str = componentStream.str();
    event->serializedBinaryData.resize(str.size());
    std::copy(str.begin(), str.end(), event->serializedBinaryData.begin());

    event->netEntityId = object()->netEntityID();

    for (uint8_t i = 0; i < object()->componentCount(); i++) {
        if (this == object()->customComponents()[i].get()) {
            event->componentIndex = i;
            break;
        }
    }

    if (auto *scn = dynamic_cast<jleSceneServer *>(scene())) {
        scn->sendNetworkEventBroadcast(std::move(event));
    } else {
        LOGE << "Component doesnt exist in server scene, failed to send event.";
    }
}
