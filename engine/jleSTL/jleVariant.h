// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/variant.h>

template <typename T>
using jleVariant = eastl::variant<T>;

namespace jle = eastl;
