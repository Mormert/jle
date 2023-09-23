// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/fixed_hash_set.h>

template <typename T, size_t nodeCount, bool bEnableOverflow = true>
using jleFixedHashSet = eastl::fixed_hash_set<T, nodeCount, bEnableOverflow>;

namespace jle = eastl;
