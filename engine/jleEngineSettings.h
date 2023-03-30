// Copyright (c) 2023. Johan Lind

#ifndef JLE_ENGINESETTINGS
#define JLE_ENGINESETTINGS

#include "jleResourceInterface.h"
#include "jleWindowSettings.h"

class jleEngineSettings : public jleResourceInterface
{
public:
    WindowSettings windowSettings;

    LOAD_THIS_SERIALIZED_JSON
    SAVE_THIS_SERIALIZED_JSON

    ~jleEngineSettings() override = default;

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(windowSettings));
    }
};

#endif // JLE_ENGINESETTINGS
