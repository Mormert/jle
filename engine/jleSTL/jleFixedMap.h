// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleBuildConfig.h"

#include <EASTL/fixed_map.h>

template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow = true>
using jleFixedMap = eastl::fixed_map<Key, T, nodeCount, bEnableOverflow>;

namespace jle = eastl;
