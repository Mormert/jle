#pragma once

#include <string>

// Wrappers for paths defined in CMakeLists

#ifdef __EMSCRIPTEN__
static std::string JLE_ENGINE_PATH { "" };
#else
static std::string JLE_ENGINE_PATH { _JLE_ENGINE_PATH };
#endif


#ifdef BUILD_EDITOR
static std::string GAME_RESOURCES_DIRECTORY { _GAME_RESOURCES_DIRECTORY };
#else
static std::string GAME_RESOURCES_DIRECTORY { "GameResources" };
#endif
