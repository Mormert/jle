// Copyright (c) 2023. Johan Lind

#include "game/jleGame.h"

#pragma once

// An empty game template that can be started in editor or as a standalone program

#if defined _WIN32 || defined __CYGWIN__
    #ifdef GAMETEMPLATE_EXPORTS
        #define GAMETEMPLATE_API __declspec(dllexport)
    #else
        #define GAMETEMPLATE_API __declspec(dllimport)
    #endif
#else
    #ifdef GAMETEMPLATE_EXPORTS
        #define GAMETEMPLATE_API __attribute__ ((visibility ("default")))
    #else
        #define GAMETEMPLATE_API
    #endif
#endif

class GAMETEMPLATE_API GameTemplate final : public jleGame
{
public:
    ~GameTemplate() override = default;

    void start(jleSerializationContext& serializationContext) override;
};
