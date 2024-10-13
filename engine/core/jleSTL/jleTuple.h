// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleBuildConfig.h"

#include <EASTL/tuple.h>

template <typename T>
using jleTuple = eastl::tuple<T>;

namespace jle = eastl;
