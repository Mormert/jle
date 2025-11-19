// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleBuildConfig.h"

#include <EASTL/intrusive_hash_map.h>

template <typename T, typename Key, size_t bucketCount>
using jleIntrusiveHashMap = eastl::intrusive_hash_map<T, Key, bucketCount>;

namespace jle = eastl;
