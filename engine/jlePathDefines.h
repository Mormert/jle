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

#include <string>

// Wrappers for paths defined in CMakeLists

#if defined(BUILD_EDITOR) || !defined(NDEBUG)
static std::string JLE_ENGINE_PATH{_JLE_ENGINE_PATH};
#else
static std::string JLE_ENGINE_PATH{""};
#endif

static std::string JLE_ENGINE_RESOURCES_PATH{JLE_ENGINE_PATH + "EngineResources"};
static std::string JLE_EDITOR_RESOURCES_PATH{JLE_ENGINE_PATH + "EditorResources"};
static std::string JLE_BINARY_RESOURCES_PATH{""};


#if defined(BUILD_EDITOR) || !defined(NDEBUG)
static std::string GAME_RESOURCES_DIRECTORY{_GAME_RESOURCES_DIRECTORY};
#else
static std::string GAME_RESOURCES_DIRECTORY{"GameResources"};
#endif

static std::string ENGINE_RESOURCES_PREFIX{"ER:"};
static std::string EDITOR_RESOURCES_PREFIX{"ED:"};
static std::string GAME_RESOURCES_PREFIX{"GR:"};
static std::string BINARY_RESOURCES_PREFIX{"BI:"};

enum class jleRootFolder {
    None,            // Opens the file path directly
    EngineResources, // Uses the prefix "ER:"
    EditorResources, // Uses the prefix "ED:"
    GameResources,   // Uses the prefix "GR:"
    BinaryFolder     // Uses the prefix "BI:"
};