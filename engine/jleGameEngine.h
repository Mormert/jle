// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleCore.h"
#include "jleGame.h"
#include <RmlUi/Core/Context.h>

class jleFullscreenRendering;
class jleFramebufferInterface;

class jleGameEngine;
inline jleGameEngine *gEngine;

class jleGameEngine : public jleCore
{
public:
    ~jleGameEngine() override;

    explicit jleGameEngine();

    template <class T>
    void
    setGame()
    {
        _gameCreator = []() { return std::make_unique<T>(); };
    }

    std::shared_ptr<jleFramebufferInterface> mainScreenFramebuffer;

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

    jlePhysics &physics();

    [[nodiscard]] bool isGameKilled() const;

    [[nodiscard]] bool isGameHalted() const;

    jleGame &gameRef();

    static inline Rml::Context* context{};

private:
    std::function<std::unique_ptr<jleGame>()> _gameCreator;

    std::unique_ptr<jleFullscreenRendering> _fullscreen_renderer;

    friend class jleGameEditorWindow;
    void gameWindowResizedEvent(unsigned int w, unsigned int h);

    std::map<unsigned int, std::function<void(unsigned int, unsigned int)>> gameWindowResizedCallbacks;

protected:
    void start() override;

    void startRmlUi();

    void killRmlUi();

    void update(float dt) override;

    void render() override;

    void renderRmlUi();

    void exiting() override;

    std::unique_ptr<jlePhysics> _physics;

    std::unique_ptr<jleGame> game;
    bool gameHalted = false;
};
