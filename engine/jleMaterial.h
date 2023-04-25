// Copyright (c) 2023. Johan Lind

#ifndef JLE_MATERIAL_H
#define JLE_MATERIAL_H

#include "jleResourceInterface.h"
#include "jleTypeReflectionUtils.h"

#include "jleResourceRef.h"
#include "jleTexture.h"
#include "json.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>


class jleMaterial : public jleResourceInterface, public std::enable_shared_from_this<jleMaterial>
{
public:

    JLE_REGISTER_RESOURCE_TYPE(jleMaterial, mat)

    virtual ~jleMaterial() = default;

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

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleResourceInterface)

    jleLoadFromFileSuccessCode
    loadFromFile(const jlePath &path) override
    {
        return jleLoadFromFileSuccessCode::IMPLEMENT_POLYMORPHIC_CEREAL;
    };

    jleResourceRef<jleTexture> albedoTextureRef;
    jleResourceRef<jleTexture> normalTextureRef;
    jleResourceRef<jleTexture> metallicTextureRef;
    jleResourceRef<jleTexture> roughnessTextureRef;

    std::shared_ptr<jleTexture> albedoTexture{};
    std::shared_ptr<jleTexture> normalTexture{};
    std::shared_ptr<jleTexture> metallicTexture{};
    std::shared_ptr<jleTexture> roughnessTexture{};

protected:
    std::string _albedoPath;
    std::string _normalPath;
    std::string _metallicPath;
    std::string _roughnessPath;
};

CEREAL_REGISTER_TYPE(jleMaterial)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleResourceInterface, jleMaterial)

#endif // JLE_MATERIAL_H