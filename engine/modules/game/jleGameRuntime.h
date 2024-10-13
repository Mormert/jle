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

class jleGame;
class jleGameEngine;
class jleTimerManager;
class jleFramebufferInterface;
struct jleEngineModulesContext;

class jleGameRuntime
{
public:
    explicit jleGameRuntime(jleGameEngine &engine);

    jleTimerManager &timerManager();

    jleGame &getGame();

    void startGame(jleEngineModulesContext &ctx);

    void restartGame(jleEngineModulesContext &ctx);

    void killGame();

    void haltGame();

    void unhaltGame();

    void executeNextFrame(jleEngineModulesContext &ctx);

    [[nodiscard]] bool isGameKilled() const;

    [[nodiscard]] bool isGameHalted() const;

    int addGameWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback);

    void removeGameWindowResizeCallback(unsigned int callbackId);

    void resizeMainFramebuffer(jleEngineModulesContext &ctx, unsigned int width, unsigned int height);

    std::unique_ptr<jleFramebufferInterface> mainGameScreenFramebuffer;

private:
    void update(jleEngineModulesContext &ctx);

    friend class jleGameEditorWindow;
    void gameWindowResizedEvent(unsigned int w, unsigned int h);
    std::unordered_map<unsigned int, std::function<void(unsigned int, unsigned int)>> _gameWindowResizedCallbacks;

    jleGameEngine &_engine;
    friend class jleGameEngine;

    std::unique_ptr<jleTimerManager> _timerManager;

    std::unique_ptr<jleGame> _game;

    std::function<std::unique_ptr<jleGame>()> _gameCreator;

    bool _gameHalted = false;
};
