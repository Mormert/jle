// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/fixed_list.h>

template <typename T, size_t nodeCount, bool bEnableOverflow = true>
using jleFixedList = eastl::fixed_list<T, nodeCount, bEnableOverflow>;

namespace jle = eastl;
