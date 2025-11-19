// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleBuildConfig.h"

#include <EASTL/shared_ptr.h>

template <typename T>
using jleSharedPtr = eastl::shared_ptr<T>;

namespace jle = eastl;
