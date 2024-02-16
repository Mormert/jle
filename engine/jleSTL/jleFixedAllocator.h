// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleBuildConfig.h"

#include <EASTL/fixed_allocator.h>

using jleFixedAllocator = eastl::fixed_allocator;
using jleFixedAllocatorWithOverflow = eastl::fixed_allocator_with_overflow;

namespace jle = eastl;
