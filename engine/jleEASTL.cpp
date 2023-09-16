// Copyright (c) 2023. Johan Lind

#ifdef WIN32
#include "Windows.h"
#undef max
#undef min

#define ASSERTION_EASTL_BREAK __debugbreak();

namespace eastl
{
void __cdecl AssertionFailure(const char *af)
{
    perror(af);
    ASSERTION_EASTL_BREAK
}
} // namespace eastl

#else

#include <cassert>
#include <cstdio>

namespace eastl
{
void __cdecl AssertionFailure(const char *af)
{
    perror(af);
    assert(0);
}
} // namespace eastl

#endif