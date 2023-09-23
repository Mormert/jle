// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/fixed_set.h>

template <typename T, int nodeCount, bool bEnableOverflow = true>
using jleFixedSet = eastl::fixed_set<T, nodeCount, bEnableOverflow>;

namespace jle = eastl;
