// Copyright (c) 2023. Johan Lind

#ifndef JLE_SKYBOX_H
#define JLE_SKYBOX_H

#include "jleSerializedResource.h"
#include "jleTypeReflectionUtils.h"

#include "jleResourceRef.h"
#include "jleImage.h"


class jleSkybox : public jleSerializedResource, public std::enable_shared_from_this<jleSkybox>
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleSkybox, skyb)

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedResource)

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_right),
           CEREAL_NVP(_left),
           CEREAL_NVP(_bottom),
           CEREAL_NVP(_top),
           CEREAL_NVP(_front),
           CEREAL_NVP(_back));
    }

    ~jleSkybox() override = default;

    jleSkybox() = default;

    jleLoadFromFileSuccessCode loadFromFile(const jlePath &path) override;

    jleResourceRef<jleImage> _right;
    jleResourceRef<jleImage> _left;
    jleResourceRef<jleImage> _bottom;
    jleResourceRef<jleImage> _top;
    jleResourceRef<jleImage> _front;
    jleResourceRef<jleImage> _back;

    unsigned int getTextureID();

    unsigned int getVAO();

protected:
    unsigned int _vao{}, _vbo{}, _textureID{};
};

CEREAL_REGISTER_TYPE(jleSkybox)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedResource, jleSkybox)

#endif // JLE_SKYBOX_H