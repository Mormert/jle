// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/intrusive_hash_set.h>

template <typename T, size_t bucketCount>
using jleIntrusiveHashSet = eastl::intrusive_hash_set<T, bucketCount>;

namespace jle = eastl;
