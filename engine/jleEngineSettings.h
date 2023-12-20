// Copyright (c) 2023. Johan Lind

#ifndef JLE_ENGINESETTINGS
#define JLE_ENGINESETTINGS

#include "jleSerializedResource.h"
#include "jleTypeReflectionUtils.h"
#include "jleWindowSettings.h"
#include "jleResourceRef.h"

class jleEngineSettings : public jleSerializedOnlyResource, public std::enable_shared_from_this<jleEngineSettings>
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleEngineSettings, "es")

    WindowSettings windowSettings;
    bool enableNetworking{true};
    std::vector<jlePath> initialScenesToLoad;

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedOnlyResource)

    ~jleEngineSettings() override = default;

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        try {
            ar(CEREAL_NVP(windowSettings), CEREAL_NVP(enableNetworking), CEREAL_NVP(initialScenesToLoad));
        } catch (std::exception &e) {
        }
    }
};

CEREAL_REGISTER_TYPE(jleEngineSettings)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleEngineSettings)

#endif // JLE_ENGINESETTINGS
