// Copyright (c) 2022. Johan Lind

#include <soloud.h>

namespace jle
{
    class jleSoLoud {
    public:
        static inline SoLoud::Soloud& GetSoLoud() { return gSoLoud; }

    private:
        friend class jleCore;
        static void Init();
        static void DeInit();

        static inline SoLoud::Soloud gSoLoud;
    };
}
