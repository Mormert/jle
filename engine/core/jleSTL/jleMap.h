// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/map.h>

template <typename Key, typename T, typename Compare = eastl::less<Key>>
using jleMap = eastl::map<Key, T, Compare>;

namespace jle = eastl;
