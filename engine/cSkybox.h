// Copyright (c) 2023. Johan Lind

#include "jleComponent.h"
#include "jleResourceRef.h"
#include "jleSkybox.h"
#include "jleRendering.h"
#include "jle3DRenderer.h"

class cSkybox : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cSkybox)
public:
    explicit cSkybox(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_skybox));

        if(_skybox.get())
        {
            gCore->renderSettings().skybox = _skybox;
        }
    }

    void start() override;

    void update(float dt) override;

protected:
    jleResourceRef<jleSkybox> _skybox;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cSkybox)

CEREAL_REGISTER_TYPE(cSkybox)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cSkybox)
