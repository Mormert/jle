// Copyright (c) 2023. Johan Lind

#ifndef JLE_MATERIAL_H
#define JLE_MATERIAL_H

#include "jleResourceInterface.h"
#include "jleTypeReflectionUtils.h"

#include "jleResourceRef.h"
#include "jleTexture.h"
#include "json.hpp"

#include <cereal/cereal.hpp>


class jleMaterial : public jleSerializedResource, public std::enable_shared_from_this<jleMaterial>
{
public:

    JLE_REGISTER_RESOURCE_TYPE(jleMaterial, mat)

    ~jleMaterial() override = default;

    jleMaterial() = default;

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(albedoTextureRef),
           CEREAL_NVP(normalTextureRef),
           CEREAL_NVP(metallicTextureRef),
           CEREAL_NVP(roughnessTextureRef));
    }

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedResource)

    std::vector<std::string> getFileAssociationList() override;

    jleResourceRef<jleTexture> albedoTextureRef;
    jleResourceRef<jleTexture> normalTextureRef;
    jleResourceRef<jleTexture> metallicTextureRef;
    jleResourceRef<jleTexture> roughnessTextureRef;
};

CEREAL_REGISTER_TYPE(jleMaterial)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedResource, jleMaterial)

#endif // JLE_MATERIAL_H