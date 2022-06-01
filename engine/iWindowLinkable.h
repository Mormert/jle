// Copyright (c) 2022. Johan Lind

#pragma once

#include "iWindowInternalAPI.h"

namespace jle {
    class iWindowLinkable {
    public:
        virtual ~iWindowLinkable() = default;

        virtual void LinkWindow(std::shared_ptr<iWindowInternalAPI> windowInternal) = 0;
    };
}