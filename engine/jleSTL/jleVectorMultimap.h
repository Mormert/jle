// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/vector_multimap.h>

template <typename Key, typename T, typename Compare = eastl::less<Key>>
using jleVectorMultimap = eastl::vector_multimap<Key, T, Compare>;

namespace jle = eastl;
