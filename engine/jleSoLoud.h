// Copyright (c) 2022. Johan Lind
#pragma once

#include <soloud.h>

class jleSoLoud {
public:
    static inline SoLoud::Soloud &soLoud() { return gSoLoud; }

private:
    friend class jleCore;

    static void init();

    static void deInit();

    static inline SoLoud::Soloud gSoLoud;
};
