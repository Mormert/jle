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

#include "jleNetworkEvent.h"
#include "jleSceneServer.h"
#include "jleSceneClient.h"

jleSceneServer &
jleClientToServerEvent::getSceneServer()
{
    return *_serverScene;
}

int32_t
jleClientToServerEvent::getClientId()
{
    return _clientId;
}

jleSceneClient &
jleServerToClientEvent::getSceneClient()
{
    return *_clientScene;
}

void
jleExecuteClientEventsOnServer(jleEngineModulesContext& ctx, const char *networkBuffer, size_t networkBufferLength, jleSceneServer *scene, int clientId)
{
    int32_t amountOfEvents;
    ::memcpy(&amountOfEvents, &networkBuffer[0], sizeof(int32_t));

    std::stringstream stream{};
    stream.write(&networkBuffer[sizeof(int32_t)], networkBufferLength - sizeof(int32_t));

    jleSerializationContext serializationContext{&ctx.resourcesModule, &ctx.luaEnvironment};
    jleBinaryInputArchive archive(stream, serializationContext);

    for (int i = 0; i < amountOfEvents; i++) {
        std::unique_ptr<jleClientToServerEvent> e{nullptr};
        try {
            archive(e);
            if (e) {
                e->_serverScene = scene;
                e->_clientId = clientId;
                e->execute(ctx);
            }
        } catch (std::exception &ex) {
            LOGE << "[client] failed to parse event data: " << ex.what();
        }
    }
}

void
jleExecuteServerEventsOnClient(jleEngineModulesContext& ctx, const char *networkBuffer, size_t networkBufferLength, jleSceneClient *scene)
{
    int32_t amountOfEvents;
    ::memcpy(&amountOfEvents, &networkBuffer[0], sizeof(int32_t));

    std::stringstream stream{};
    stream.write(&networkBuffer[sizeof(int32_t)], networkBufferLength - sizeof(int32_t));

    jleSerializationContext serializationContext{&ctx.resourcesModule, &ctx.luaEnvironment};
    jleBinaryInputArchive archive(stream, serializationContext);

    for (int i = 0; i < amountOfEvents; i++) {
        std::unique_ptr<jleServerToClientEvent> e{nullptr};
        try {
            archive(e);
            if (e) {
                e->_clientScene = scene;
                e->execute(ctx);
            }
        } catch (std::exception &ex) {
            LOGE << "[client] failed to parse event data: " << ex.what();
        }
    }
}
