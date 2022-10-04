// Copyright (c) 2022. Johan Lind

#include "jleSoLoud.h"
#include <iostream>

void jleSoLoud::init() {
#ifndef NDEBUG
    std::cout << "Initializing Sound Engine\n";
#endif
    gSoLoud.init();
}

void jleSoLoud::deInit() {
#ifndef NDEBUG
    std::cout << "De-initializing Sound Engine\n";
#endif
    gSoLoud.deinit();
}
