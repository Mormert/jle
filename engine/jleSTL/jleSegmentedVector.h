// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/segmented_vector.h>
#include <EASTL/allocator.h>

template <typename T, size_t Count, typename Allocator = eastl::allocator>
using jleSegmentedVector = eastl::segmented_vector<T, Count, Allocator>;

namespace jle = eastl;
