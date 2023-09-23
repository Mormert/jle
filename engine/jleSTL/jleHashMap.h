// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/hash_map.h>

template <typename Key, typename T>
using jleHashMap = eastl::hash_map<Key, T>;

namespace jle = eastl;
