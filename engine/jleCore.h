// Copyright (c) 2022. Johan Lind

#pragma once

#include "no_copy_no_move.h"
#include "iWindowAPI.h"
#include "InputAPI.h"
#include "iRenderingAPI.h"
#include "iFramebuffer.h"
#include "iTextureCreator.h"
#include "iRenderingFactory.h"
#include "iWindowFactory.h"
#include "iWindowInitializer.h"
#include "jleCoreSettings.h"
#include "jleTimerManager.h"

#include <memory>

namespace jle {

    struct jleCoreStatus {
        virtual ~jleCoreStatus() = default;

        virtual int GetFPS() = 0;

        virtual float GetDeltaFrameTime() = 0;

        virtual float GetCurrentFrameTime() = 0;

        virtual float GetLastFrameTime() = 0;
    };

    // Core part of the jle engine
    class jleCore {
    public:
        NO_COPY_NO_MOVE(jleCore)

        explicit jleCore(const std::shared_ptr<jleCoreSettings> &cs);

        virtual ~jleCore();

        void Run();

        const std::unique_ptr<iRenderingFactory> renderingFactory;
        const std::unique_ptr<iWindowFactory> windowFactory;

        // Singleton
        static jleCore *core;

        // Entry point for a user to access the windowing API
        const std::shared_ptr<iWindowAPI> window;

        // Entry point for a user to access the input API
        const std::shared_ptr<InputAPI> input;

        // Entry point for a user to do fundamental rendering
        const std::shared_ptr<iRenderingAPI> rendering;

        // Entry point for a user to create textures of different kinds
        const std::shared_ptr<iTextureCreator> texture_creator;

        // Entry point for a user to get core status
        const std::shared_ptr<jleCoreStatus> status;

        jleTimerManager& GetTimerManager();

    private:
        void Loop();

        void MainLoop();

        bool running{false};

        static void main_loop() {
            jleCore::core->MainLoop();
        }

        const std::unique_ptr<iWindowInitializer> window_initializer;

        // Internal impl data
        struct jleCoreInternalImpl;
        std::unique_ptr<jleCoreInternalImpl> coreImpl;

        friend struct CoreStatus_Internal;

    protected:
        virtual void Start() {}

        virtual void Update(float dt) {}

        virtual void Render() {}

        virtual void Exiting() {}

        std::shared_ptr<jleCoreSettings> core_settings;

        jleTimerManager mTimerManager;
    };
}