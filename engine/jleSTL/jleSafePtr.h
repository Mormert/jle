// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/safe_ptr.h>

template <typename T>
using jleSafePtr = eastl::safe_ptr<T>;

namespace jle = eastl;
