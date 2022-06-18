// Copyright (c) 2022. Johan Lind

#include "jleSoLoud.h"
#include <plog/Log.h>

void jle::jleSoLoud::Init() {
    LOGI << "Initializing Sound Engine";
    gSoLoud.init();
}

void jle::jleSoLoud::DeInit() {
    LOGI << "De-initializing Sound Engine";
    gSoLoud.deinit();
}
