// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/intrusive_list.h>

template <typename T>
using jleIntrusiveList = eastl::intrusive_list<T>;

namespace jle = eastl;
