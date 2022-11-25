// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCore.h"

#include "jleGame.h"
#include "jleGameSettings.h"

class jleFullscreenRendering;
class jleFramebuffer;

class jleGameEngine;
inline jleGameEngine *gEngine;

class jleGameEngine : public jleCore
{
public:
    ~jleGameEngine() override;

    explicit jleGameEngine(std::shared_ptr<jleGameSettings> gs);

    template <class T>
    void
    setGame()
    {
        _gameCreator = []() { return std::make_unique<T>(); };
    }

    std::shared_ptr<jleFramebuffer> mainRenderFramebuffer;

    void resizeMainFramebuffer(unsigned int width, unsigned int height);

    int addGameWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback);

    void removeGameWindowResizeCallback(unsigned int callbackId);

    void executeGameWindowResizedCallbacks(unsigned int w, unsigned int h);

    void startGame();

    void restartGame();

    void killGame();

    void haltGame();

    void unhaltGame();

    void executeNextFrame();

    [[nodiscard]] bool isGameKilled() const;

    [[nodiscard]] bool isGameHalted() const;

    jleGame &gameRef();

private:
    std::function<std::unique_ptr<jleGame>()> _gameCreator;

    std::unique_ptr<jleFullscreenRendering> _fullscreen_renderer;

    friend class jleGameEditorWindow;
    void gameWindowResizedEvent(unsigned int w, unsigned int h);

    std::map<unsigned int, std::function<void(unsigned int, unsigned int)>> gameWindowResizedCallbacks;

protected:
    void start() override;

    void update(float dt) override;

    void render() override;

    void exiting() override;

    std::shared_ptr<jleGameSettings> gameSettings;

    std::unique_ptr<jleGame> game;
    bool gameHalted = false;
};
