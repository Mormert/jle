// Copyright (c) 2023. Johan Lind

#include "jleComponent.h"
#include "jleSkybox.h"

class cSkybox : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cSkybox)
public:
    explicit cSkybox(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_skyboxPath));
    }

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

protected:
    std::shared_ptr<jleSkybox> _skybox;
    std::string _skyboxPath;
};

CEREAL_REGISTER_TYPE(cSkybox)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cSkybox)
