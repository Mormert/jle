// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/string_hash_map.h>

template <typename T>
using jleStringHashMap = eastl::string_hash_map<T>;

namespace jle = eastl;
