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

#include <functional>
#include <memory>

class jleSerializableInterface;
struct jleSerializationContext;
class jleGame;
struct jleGameModules;
class jleGameEngine;
struct jleEngineUpdateContext;

namespace jlECS{
class ECS;
namespace Debug
{
class ECS_Debug;
}
}

struct jleGameConstructConfig
{
    std::function<std::unique_ptr<jleGameModules>(bool /*gameRunning*/)> modulesCreator = {};
    std::function<std::unique_ptr<jlECS::ECS>()> ecsCreator = {};
    std::function<void(jleGameModules&, jlECS::ECS&, jleSerializationContext&)> modulesInitialize = {};
    std::function<void(jleGameModules&, jleEngineUpdateContext&, jlECS::ECS&)> modulesUpdate = {};
    std::function<void(jleGameModules&, std::vector<jleSerializableInterface*>&)> populateSerializationInterfaces = {};
    std::function<void(jleGameModules&, jleSerializationContext&)> modulesPreRender = {};
    std::function<bool(jleGameModules&, jleSerializationContext&)> modulesRender = {};
    std::function<void(jleGameModules&, jleSerializationContext&)> modulesPostRender = {};
};

class jleGameRuntime
{
public:
    jleGameRuntime(const jleGameConstructConfig &config);

    jleGame &getGame();

    void startGame(jleSerializationContext& serializationContext);

    void restartGame(jleSerializationContext& serializationContext);

    void killGame();

    void haltGame();

    void unhaltGame();

    void executeNextFrame(jleEngineUpdateContext &ctx);

    [[nodiscard]] bool isGameKilled() const;

    [[nodiscard]] bool isGameHalted() const;

    void update(jleEngineUpdateContext &ctx);

    void processGameReset(jleEngineUpdateContext &ctx);

private:
    std::unique_ptr<jleGame> _game{};

    jleGameConstructConfig _gameConstructConfig{};

    bool _gameHalted = false;
    bool _gameIsGettingKilled = false;
    bool _gameIsGettingRestarted = false;
};
