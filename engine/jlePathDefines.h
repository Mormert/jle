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

#if defined(JLE_BUILD_EDITOR) || !defined(NDEBUG)

#define JLE_Q(x) #x
#define JLE_QUOTE(x) JLE_Q(x)

static const std::string JLE_ENGINE_PATH{JLE_QUOTE(JLE_ENGINE_PATH_CMAKE)};
#else
static const std::string JLE_ENGINE_PATH{""};
#endif

static const std::string JLE_ENGINE_RESOURCES_PATH{JLE_ENGINE_PATH + "EngineResources"};
static const std::string JLE_EDITOR_RESOURCES_PATH{JLE_ENGINE_PATH + "EditorResources"};
static const std::string JLE_BINARY_RESOURCES_PATH{""};


#if defined(JLE_BUILD_EDITOR) || !defined(NDEBUG)
static const std::string GAME_RESOURCES_DIRECTORY{JLE_QUOTE(GAME_RESOURCES_DIRECTORY_CMAKE)};
#else
static std::string GAME_RESOURCES_DIRECTORY{"GameResources"};
#endif

static const std::string ENGINE_RESOURCES_PREFIX{"ER:"};
static const std::string EDITOR_RESOURCES_PREFIX{"ED:"};
static const std::string GAME_RESOURCES_PREFIX{"GR:"};
static const std::string BINARY_RESOURCES_PREFIX{"BI:"};

enum class jleRootFolder {
    None,            // Opens the file path directly
    EngineResources, // Uses the prefix "ER:"
    EditorResources, // Uses the prefix "ED:"
    GameResources,   // Uses the prefix "GR:"
    BinaryFolder     // Uses the prefix "BI:"
};