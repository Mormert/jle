// Copyright (c) 2022. Johan Lind

#include "jleSoLoud.h"
#include <iostream>

void jle::jleSoLoud::Init() {
#ifndef NDEBUG
    std::cout << "Initializing Sound Engine\n";
#endif
    gSoLoud.init();
}

void jle::jleSoLoud::DeInit() {
#ifndef NDEBUG
    std::cout << "De-initializing Sound Engine\n";
#endif
    gSoLoud.deinit();
}