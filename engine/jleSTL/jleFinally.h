// Copyright (c) 2023. Johan Lind

#pragma once

#include <EASTL/finally.h>

template <typename Functor>
using jleFinally = eastl::finally<Functor>;

namespace jle = eastl;
