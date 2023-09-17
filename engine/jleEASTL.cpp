// Copyright (c) 2023. Johan Lind

#ifdef WIN32
#include "Windows.h"
#undef max
#undef min

#define ASSERTION_EASTL_BREAK __debugbreak();

#ifndef JLE_CDECL
#ifdef _MSC_VER
#define JLE_CDECL __cdecl
#else
#define JLE_CDECL
#endif
#endif

namespace eastl
{
void JLE_CDECL AssertionFailure(const char *af)
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
void JLE_CDECL AssertionFailure(const char *af)
{
    perror(af);
    assert(0);
}
} // namespace eastl

#endif