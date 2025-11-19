// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/fixed_vector.h>

template <typename T, size_t nodeCount, bool bEnableOverflow = true>
using jleFixedVector = eastl::fixed_vector<T, nodeCount, bEnableOverflow>;

namespace jle = eastl;
