// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/unordered_map.h>

template <typename Key, typename T, typename Compare = eastl::less<Key>>
using jleUnorderedMap = eastl::unordered_map<Key, T, Compare>;

namespace jle = eastl;
