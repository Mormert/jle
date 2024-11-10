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

#include "jleNetworkingComponent.h"

#include "jleNetworkEvent.h"
#include "jleSceneClient.h"
#include "jleSceneServer.h"

struct jleComponentNetSyncEvent : public jleServerToClientEvent {
    void
    execute(jleEngineModulesContext& ctx) override
    {
        auto &scene = getSceneClient();
        if (auto object = scene.getObjectFromNetId(netEntityId)) {
            if (componentIndex > object->componentCount()) {
                return;
            }
            auto component = object->components()[componentIndex];
            try {
                std::stringstream stream{};
                stream.write(&serializedBinaryData[0], serializedBinaryData.size());

                jleSerializationContext serializationContext{&ctx.resourcesModule, &ctx.luaEnvironment, &ctx.renderThread};
                jleBinaryInputArchive archive(stream, serializationContext);
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
syncComponentServerToClient(jleComponent &component, jleSerializationContext &ctx)
{
    auto event = jleMakeNetEvent<jleComponentNetSyncEvent>();

    std::stringstream componentStream{};
    {
        jleBinaryOutputArchive componentArchive(componentStream, ctx);
        component.netSyncOut(componentArchive);
    }

    const auto &str = componentStream.str();
    event->serializedBinaryData.resize(str.size());
    std::copy(str.begin(), str.end(), event->serializedBinaryData.begin());

    event->netEntityId = component.object()->netID();

    for (uint8_t i = 0; i < component.object()->componentCount(); i++) {
        if (&component == component.object()->components()[i].get()) {
            event->componentIndex = i;
            break;
        }
    }

    if (auto *scn = dynamic_cast<jleSceneServer *>(component.scene())) {
        scn->sendNetworkEventBroadcast(std::move(event));
    } else {
        LOGE << "Component doesnt exist in server scene, failed to send event.";
    }
}






