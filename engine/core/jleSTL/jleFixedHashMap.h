// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/fixed_hash_map.h>

template <typename Key, typename T, size_t nodeCount, bool bEnableOverflow = true>
using jleFixedHashMap = eastl::fixed_hash_map<Key, T, nodeCount, bEnableOverflow>;

namespace jle = eastl;
