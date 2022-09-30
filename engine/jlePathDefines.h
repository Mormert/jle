// Copyright (c) 2022. Johan Lind

#pragma once

#include "plog/Log.h"
#include <regex>
#include <string>

// Wrappers for paths defined in CMakeLists

#ifndef BUILD_EDITOR
static std::string JLE_ENGINE_PATH{""};
#else
static std::string JLE_ENGINE_PATH{_JLE_ENGINE_PATH};
#endif

static std::string JLE_ENGINE_RESOURCES_PATH{JLE_ENGINE_PATH +
                                             "EngineResources"};
static std::string JLE_EDITOR_RESOURCES_PATH{JLE_ENGINE_PATH +
                                             "EditorResources"};

#ifdef BUILD_OPENGLES30
static std::string JLE_ENGINE_PATH_SHADERS{JLE_ENGINE_RESOURCES_PATH +
                                           "/shaders/es"};
#else
static std::string JLE_ENGINE_PATH_SHADERS{JLE_ENGINE_RESOURCES_PATH +
                                           "/shaders/core"};
#endif

#ifdef BUILD_EDITOR
static std::string GAME_RESOURCES_DIRECTORY{_GAME_RESOURCES_DIRECTORY};
#else
static std::string GAME_RESOURCES_DIRECTORY{"GameResources"};
#endif

static std::string ENGINE_RESOURCES_PREFIX{"ER:"};
static std::string EDITOR_RESOURCES_PREFIX{"ED:"};
static std::string GAME_RESOURCES_PREFIX{"GR:"};

enum class jleRootFolder {
    None,            // Opens the file path directly
    EngineResources, // Uses the prefix "ER:"
    EditorResources, // Uses the prefix "ED:"
    GameResources    // Uses the prefix "GR:"
};