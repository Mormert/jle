// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/deque.h>

template <typename T>
using jleDeque = eastl::decay_t<T>;

namespace jle = eastl;
