// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/fixed_substring.h>

template <typename T>
using jleFixedSubstring = eastl::fixed_substring<T>;
using jleFixedSubstringChar = eastl::fixed_substring<char>;

namespace jle = eastl;
