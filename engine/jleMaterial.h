// Copyright (c) 2023. Johan Lind

#ifndef JLE_MATERIAL_H
#define JLE_MATERIAL_H

#include "jleCompileHelper.h"
#include "jleResourceInterface.h"
#include "jleTypeReflectionUtils.h"

#include "jle3DGraph.h"
#include "jle3DSettings.h"
#include "jleCamera.h"
#include "jleResourceRef.h"
#include "jleShader.h"
#include "jleTexture.h"
#include "jleTextureRefOrRGBA.h"

#include "editor/jleImGuiCerealArchive.h"
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>

class jleMaterial : public jleSerializedResource, public std::enable_shared_from_this<jleMaterial>
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleMaterial, mat)

    ~jleMaterial() override = default;

    jleMaterial() = default;

    virtual void useMaterial(const jleCamera &camera,
                             const std::vector<jle3DRendererLight> &lights,
                             const jle3DSettings &settings);

    template <class Archive>
    void serialize(Archive &ar);

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedResource)

    std::vector<std::string> getFileAssociationList() override;

    std::shared_ptr<jleShader> getShader();

protected:
    jleResourceRef<jleShader> _shaderRef;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleMaterial)

CEREAL_REGISTER_TYPE(jleMaterial)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedResource, jleMaterial)

class jleMaterialPBR : public jleMaterial
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleMaterialPBR, mat)

    void useMaterial(const jleCamera &camera,
                     const std::vector<jle3DRendererLight> &lights,
                     const jle3DSettings &settings) override;

    template <class Archive>
    void serialize(Archive &ar);

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleMaterial)

    jleTextureRefOrRGB _albedo{glm::vec3{1.f}};           // White
    jleTextureRefOrRGB _normal{glm::vec3{0.f, 0.f, 1.f}}; // Normals pointing up
    jleTextureRefOrAlpha _metallic{0.0f};                 // No metallic
    jleTextureRefOrAlpha _roughness{0.5f};                // Half roughness
    bool _usePointShadows{true};
    bool _useDirectionalShadows{true};
    bool _useSkyboxEnvironmentMap{false};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleMaterialPBR)

CEREAL_REGISTER_TYPE(jleMaterialPBR)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleMaterial, jleMaterialPBR)

#endif // JLE_MATERIAL_H