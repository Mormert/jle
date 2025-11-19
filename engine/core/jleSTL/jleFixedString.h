// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/fixed_string.h>

template <typename T, int nodeCount, bool bEnableOverflow = true>
using jleFixedString = eastl::fixed_string<T, nodeCount, bEnableOverflow>;

template <int nodeCount, bool bEnableOverflow = true>
using jleFixedStringChar = eastl::fixed_string<char, nodeCount, bEnableOverflow>;

namespace jle = eastl;
