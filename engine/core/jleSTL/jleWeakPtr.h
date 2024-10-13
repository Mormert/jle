// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/weak_ptr.h>

template <typename T>
using jleWeakPtr = eastl::weak_ptr<T>;

namespace jle = eastl;
