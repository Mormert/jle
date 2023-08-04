#pragma once

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

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include "cereal/details/helpers.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/polymorphic.hpp"

#include <plog/Log.h>
#include <plog/Logger.h>

#define SOL_ALL_SAFETIES_ON 1
#include <sol2/sol.hpp>

#include "glm/glm.hpp"
#include "glm/common.hpp"
