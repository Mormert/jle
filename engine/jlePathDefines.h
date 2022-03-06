#pragma once

#include <string>

// Wrappers for paths defined in CMakeLists
static std::string JLE_ENGINE_PATH { _JLE_ENGINE_PATH };

#ifdef NDEBUG
static std::string GAME_RESOURCES_DIRECTORY { "GameResources" };
#else
static std::string GAME_RESOURCES_DIRECTORY { _GAME_RESOURCES_DIRECTORY };
#endif
