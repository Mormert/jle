// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/span.h>

template <typename T, size_t Extent = eastl::dynamic_extent>
using jleSpan = eastl::span<T, Extent>;

namespace jle = eastl;
