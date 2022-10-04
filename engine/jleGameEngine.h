// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCore.h"

#include "jleGame.h"
#include "jleGameSettings.h"

class jleFullscreenRendering;
class jleFramebuffer;

class jleGameEngine : public jleCore {
public:
    ~jleGameEngine() override;

    explicit jleGameEngine(std::shared_ptr<jleGameSettings> gs);

    template <class T>
    void SetGame() {
        _gameCreator = []() { return std::make_unique<T>(); };
    }

    // Main framebuffer
    std::shared_ptr<jleFramebuffer> framebuffer_main;

    void SetGameDimsPixels(FIXED_AXIS fa, unsigned int pixels);

    std::pair<unsigned int, unsigned int> framebufferDimensions(
        unsigned int windowWidth, unsigned int windowHeight);

    void startGame();

    void restartGame();

    void killGame();

    void haltGame();

    void unhaltGame();

    void executeNextFrame();

    bool isGameKilled();

    bool isGameHalted();

    jleGame& gameRef();

    static inline jleGameEngine *gEngine;

private:
    std::function<std::unique_ptr<jleGame>()> _gameCreator;

    std::unique_ptr<jleFullscreenRendering> _fullscreen_renderer;

    void framebufferResizeEvent(unsigned int width, unsigned int height);

protected:
    void start() override;

    void update(float dt) override;

    void Render() override;

    void exiting() override;

    FIXED_AXIS fixed_axis;

    // Game dimensions in pixels, along axis specified by fixed_axis.
    unsigned int gameDimsPixels;

    std::shared_ptr<jleGameSettings> gameSettings;

    std::unique_ptr<jleGame> game;
    bool gameHalted = false;
};
