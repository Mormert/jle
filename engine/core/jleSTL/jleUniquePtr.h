// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/unique_ptr.h>

template <typename T>
using jleUniquePtr = eastl::unique_ptr<T>;

namespace jle = eastl;
