// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/fixed_slist.h>

template <typename T, int nodeCount, bool bEnableOverflow = true>
using jleFixedSlist = eastl::fixed_slist<T, nodeCount, bEnableOverflow>;

namespace jle = eastl;
