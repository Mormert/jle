// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/bitset.h>

template <size_t N, typename WordType = EASTL_BITSET_WORD_TYPE_DEFAULT>
using jleBitset = eastl::bitset<N, WordType>;

namespace jle = eastl;
