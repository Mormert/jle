// Copyright (c) 2023. Johan Lind

#ifndef JLE_ENGINESETTINGS
#define JLE_ENGINESETTINGS

#include "jleResourceInterface.h"
#include "jleWindowSettings.h"

class jleEngineSettings : public jleResourceInterface, public std::enable_shared_from_this<jleEngineSettings>
{
public:
    WindowSettings windowSettings;

    LOAD_THIS_SERIALIZED_JSON
    SAVE_SHARED_THIS_SERIALIZED_JSON(jleResourceInterface)

    ~jleEngineSettings() override = default;

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(windowSettings));
    }
};

CEREAL_REGISTER_TYPE(jleEngineSettings)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleResourceInterface, jleEngineSettings)


#endif // JLE_ENGINESETTINGS
