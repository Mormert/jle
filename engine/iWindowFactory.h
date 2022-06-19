// Copyright (c) 2022. Johan Lind

#pragma once

#include "iWindowAPI.h"

#include "iWindowInitializer.h"

#include <memory>

namespace jle {
    class iWindowFactory {
    public:
        virtual ~iWindowFactory() = default;

        virtual std::unique_ptr<iWindowAPI> CreateWin() = 0;

        virtual std::unique_ptr<iWindowInitializer> CreateWindowInitializer() = 0;
    };
}

