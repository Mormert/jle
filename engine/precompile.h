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

#include "jleCommon.h"

#include <typeinfo>
#include <functional>
#include <iterator>
#include <limits>
#include <string>
#include <memory>
#include <random>
#include <chrono>
#include <type_traits>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <variant>

#include "jleArray.h"
#include "jleVector.h"
#include "jleAlgorithm.h"
#include "jleDeque.h"
#include "jleHashMap.h"
#include "jleUniquePtr.h"
#include "jleSharedPtr.h"
#include "jleString.h"
#include "jleStringView.h"

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include "cereal/details/helpers.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/polymorphic.hpp"

#include <plog/Log.h>
#include <plog/Logger.h>

#define SOL_ALL_SAFETIES_ON 1
#include <sol2/sol.hpp>

#include "glm/glm.hpp"
#include "glm/common.hpp"
