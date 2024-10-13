// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/fixed_function.h>

template <int SIZE_IN_BYTES, typename R, typename... Args>
using jleFixedFunction = eastl::fixed_function<SIZE_IN_BYTES, R(Args...)>;

namespace jle = eastl;
