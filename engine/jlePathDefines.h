// Copyright (c) 2022. Johan Lind

#pragma once

#include <string>
#include "plog/Log.h"

// Wrappers for paths defined in CMakeLists

#ifndef BUILD_EDITOR
static std::string JLE_ENGINE_PATH { "" };
#else
static std::string JLE_ENGINE_PATH{_JLE_ENGINE_PATH};
#endif

static std::string JLE_ENGINE_RESOURCES_PATH{JLE_ENGINE_PATH + "EngineResources"};

#ifdef BUILD_OPENGLES30
static std::string JLE_ENGINE_PATH_SHADERS{JLE_ENGINE_RESOURCES_PATH + "/shaders/es"};
#else
static std::string JLE_ENGINE_PATH_SHADERS{JLE_ENGINE_RESOURCES_PATH + "/shaders/core"};
#endif

#ifdef BUILD_EDITOR
static std::string GAME_RESOURCES_DIRECTORY{_GAME_RESOURCES_DIRECTORY};
#else
static std::string GAME_RESOURCES_DIRECTORY{"GameResources"};
#endif

static std::string ENGINE_RESOURCES_PREFIX{"ER:"};
static std::string GAME_RESOURCES_PREFIX{"GR:"};

enum class jleRootFolder {
    None,                   // Opens the file path directly
    EngineResources,        // Uses the prefix "ER:"
    GameResources           // Uses the prefix "GR:"
};

namespace jle {
    // From a resource path containing for example the module "GR:" (game resource),
    // This function finds the true path to that resource
    static std::string FindTrueResourcePath(std::string path) {

        jleRootFolder rootFolder = jleRootFolder::None;
        const auto &&prefixString = path.substr(0, 3);

        if (prefixString == "GR:") {
            rootFolder = jleRootFolder::GameResources;
        } else if (prefixString == "ER:") {
            rootFolder = jleRootFolder::EngineResources;
        }

        std::string rootFolderStr;
        std::string *resourcesDirectory;
        switch (rootFolder) {
            case jleRootFolder::EngineResources:
                rootFolderStr = ENGINE_RESOURCES_PREFIX;
                resourcesDirectory = &JLE_ENGINE_RESOURCES_PATH;
                break;
            case jleRootFolder::GameResources:
                rootFolderStr = GAME_RESOURCES_PREFIX;
                resourcesDirectory = &GAME_RESOURCES_DIRECTORY;
                break;
            case jleRootFolder::None:
                return path;
        }

        if (path.find(rootFolderStr) == std::string::npos) {
            LOG_ERROR << "Could not find true game resource path. Path did not contain " << rootFolderStr;
            return path;
        }

        // Remove the root folder prefix
        path.erase(0, 3);

        return *resourcesDirectory + '/' + path;
    }

    static std::string GenerateTrueResourcePathFromAbsolute(std::string path)
    {
        const std::string gameResourcesStr {"GameResources"};

        int gameResoures = path.find(gameResourcesStr);
        if(gameResoures >= 0)
        {
            path.erase(0, gameResoures + gameResourcesStr.length());
            const std::string retpath = "GR:" + path;
            return retpath;
        }

        const std::string engineResourcesStr {"EngineResources"};
        int engineResoures = path.find(engineResourcesStr);
        if(engineResoures >= 0)
        {
            path.erase(0, engineResoures + engineResourcesStr.length());
            const std::string retpath = "ER:" + path;
            return retpath;
        }

        LOGE << "Could not generate true resource path from absolute path for " << path;
        return path;

    }
}
