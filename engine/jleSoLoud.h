// Copyright (c) 2022. Johan Lind

#include <soloud.h>

class jleSoLoud {
public:
    static inline SoLoud::Soloud& soLoud() { return gSoLoud; }

private:
    friend class jleCore;

    static void Init();

    static void deInit();

    static inline SoLoud::Soloud gSoLoud;
};
