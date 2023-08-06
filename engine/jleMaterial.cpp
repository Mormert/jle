// Copyright (c) 2023. Johan Lind
#include "jleMaterial.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleMaterial)
JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleMaterialPBR)

std::vector<std::string>
jleMaterial::getFileAssociationList()
{
    return {"mat"};
}

void
jleMaterial::useMaterial(const jleCamera &camera,
                         const std::vector<jle3DRendererLight> &lights,
                         const jle3DSettings &settings)
{
}

std::shared_ptr<jleShader>
jleMaterial::getShader()
{
    return _shaderRef.get();
}

template <class Archive>
void
jleMaterial::serialize(Archive &ar)
{
    ar(CEREAL_NVP(_shaderRef));
}
void
jleMaterialPBR::useMaterial(const jleCamera &camera,
                            const std::vector<jle3DRendererLight> &lights,
                            const jle3DSettings &settings)
{
    auto &shader = *_shaderRef.get();

    shader.use();
    shader.SetTextureSlot("shadowMap", jleTextureSlot::DirectionalShadow);
    shader.SetTextureSlot("shadowMapPoint", jleTextureSlot::PointShadow);
    shader.SetTextureSlot("albedoTexture", jleTextureSlot::Albedo);
    shader.SetTextureSlot("normalTexture", jleTextureSlot::Normal);
    shader.SetTextureSlot("skyboxTexture", jleTextureSlot::Skybox);
    shader.SetFloat("farPlane", 500.f);
    shader.SetBool("UseDirectionalLight", settings.useDirectionalLight);
    shader.SetBool("UseEnvironmentMapping", settings.useEnvironmentMapping && _useSkyboxEnvironmentMap);
    shader.SetVec3("DirectionalLightColour", settings.directionalLightColour);
    shader.SetVec3("DirectionalLightDir", settings.directionalLightRotation);
    shader.SetMat4("view", camera.getViewMatrix());
    shader.SetMat4("proj", camera.getProjectionMatrix());
    shader.SetMat4("lightSpaceMatrix", settings.lightSpaceMatrix);
    shader.SetVec3("CameraPosition", camera.getPosition());
    shader.SetInt("LightsCount", (int)lights.size());

    // Limit to 4 lights
    int lightCount = lights.size();
    if (lightCount > 4) {
        lightCount = 4;
    }

    for (int l = 0; l < lightCount; l++) {
        shader.SetVec3("LightPositions[" + std::to_string(l) + "]", lights[l].position);
        shader.SetVec3("LightColors[" + std::to_string(l) + "]", lights[l].color);
    }

    if (_albedo.textureRef()) {
        _albedo.textureRef()->setActive(jleTextureSlot::Albedo);
        shader.SetBool("useAlbedoTexture", true);
    } else {
        shader.SetBool("useAlbedoTexture", false);
        shader.SetVec3("albedo", _albedo.color());
    }

    if (_normal.textureRef()) {
        _normal.textureRef()->setActive(jleTextureSlot::Normal);
        shader.SetBool("useNormalTexture", true);
    } else {
        shader.SetBool("useNormalTexture", false);
        shader.SetVec3("normal", _normal.color());
    }

    if (_metallic.textureRef()) {
        _metallic.textureRef()->setActive(jleTextureSlot::Metallic);
        shader.SetBool("useMetallicTexture", true);
    } else {
        shader.SetBool("useMetallicTexture", false);
        shader.SetFloat("metallic", _metallic.alpha());
    }

    if (_roughness.textureRef()) {
        _roughness.textureRef()->setActive(jleTextureSlot::Rougness);
        shader.SetBool("useRoughnessTexture", true);
    } else {
        shader.SetBool("useRoughnessTexture", false);
        shader.SetFloat("roughness", _roughness.alpha());
    }
}

template <class Archive>
void
jleMaterialPBR::serialize(Archive &ar)
{
    try {
        ar(cereal::base_class<jleMaterial>(this),
           CEREAL_NVP(_albedo),
           CEREAL_NVP(_normal),
           CEREAL_NVP(_metallic),
           CEREAL_NVP(_roughness),
           CEREAL_NVP(_usePointShadows),
           CEREAL_NVP(_useDirectionalShadows),
           CEREAL_NVP(_useSkyboxEnvironmentMap));
    } catch (std::exception &e) {
        LOGE << "Failed loading material:" << e.what();
    }
}
