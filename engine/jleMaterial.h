// Copyright (c) 2023. Johan Lind

#ifndef JLE_MATERIAL_H
#define JLE_MATERIAL_H

#include "jleResourceInterface.h"

#include "jleResourceRef.h"
#include "jleTexture.h"
#include "json.hpp"

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>


class jleMaterial : public jleResourceInterface
{
public:
    ~jleMaterial() override;

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

    jleLoadFromFileSuccessCode loadFromFile(const jlePath &path) override;

    void saveToFile() override;

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

#endif // JLE_MATERIAL_H