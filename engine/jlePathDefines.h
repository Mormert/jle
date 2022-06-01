// Copyright (c) 2022. Johan Lind

#pragma once

#include <string>
#include "plog/Log.h"

// Wrappers for paths defined in CMakeLists

#ifdef __EMSCRIPTEN__
static std::string JLE_ENGINE_PATH { "" };
#else
static std::string JLE_ENGINE_PATH{_JLE_ENGINE_PATH};
#endif


#ifdef BUILD_EDITOR
static std::string GAME_RESOURCES_DIRECTORY{_GAME_RESOURCES_DIRECTORY};
#else
static std::string GAME_RESOURCES_DIRECTORY { "GameResources" };
#endif

static std::string ENGINE_RESOURCES_PREFIX{"ER:"};
static std::string GAME_RESOURCES_PREFIX{"GR:"};

enum class jleRootFolder {
    EngineResources,
    GameResources
};

namespace jle {
    // From a resource path containing for example the module "GR:" (game resource),
    // This function finds the true path to that resource
    static std::string FindTrueResourcePath(jleRootFolder rootFolder, std::string path) {

        std::string rootFolderStr;
        std::string *resourcesDirectory;
        switch (rootFolder) {
            case jleRootFolder::EngineResources:
                rootFolderStr = ENGINE_RESOURCES_PREFIX;
                resourcesDirectory = &JLE_ENGINE_PATH;
                break;
            case jleRootFolder::GameResources:
                rootFolderStr = GAME_RESOURCES_PREFIX;
                resourcesDirectory = &GAME_RESOURCES_DIRECTORY;
                break;
        }

        if (path.find(rootFolderStr) == std::string::npos) {
            LOG_ERROR << "Could not find true game resource path. Path did not contain " << rootFolderStr;
            return path;
        }

        // Remove the root folder prefix
        path.erase(0, 3);

        return *resourcesDirectory + '/' + path;
    }
}
