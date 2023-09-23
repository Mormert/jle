// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/vector_map.h>

template <typename Key, typename T, typename Compare = eastl::less<Key>>
using jleVectorMap = eastl::vector_map<Key, T, Compare>;

namespace jle = eastl;
