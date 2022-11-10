// Copyright (c) 2022. Johan Lind

#include "jleComponent.h"
#include "jleSkybox.h"

class cSkybox : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cSkybox)
public:
    explicit cSkybox(jleObject *owner = nullptr, jleScene *scene = nullptr);

    void start() override;

    void update(float dt) override;

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

protected:
    std::shared_ptr<jleSkybox> _skybox;
    std::string _skyboxPath;
};
