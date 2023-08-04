// Copyright (c) 2023. Johan Lind
#include "jleMaterial.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleMaterial)

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

    auto &shader = *_shaderRef.get();

    shader.use();
    shader.SetTextureSlot("shadowMap", jleTextureSlot::DirectionalShadow);
    shader.SetTextureSlot("shadowMapPoint", jleTextureSlot::PointShadow);
    shader.SetTextureSlot("albedoTexture", jleTextureSlot::Albedo);
    shader.SetTextureSlot("normalTexture", jleTextureSlot::Normal);
    shader.SetTextureSlot("skyboxTexture", jleTextureSlot::Skybox);
    shader.SetFloat("farPlane", 500.f);
    shader.SetBool("UseDirectionalLight", settings.useDirectionalLight);
    shader.SetBool("UseEnvironmentMapping", settings.useEnvironmentMapping);
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

    if (_albedoTextureRef) {
        _albedoTextureRef->setActive(jleTextureSlot::Albedo);
        shader.SetBool("useAlbedoTexture", true);
    } else {
        shader.SetBool("useAlbedoTexture", false);
    }

    if (_normalTextureRef) {
        _normalTextureRef->setActive(jleTextureSlot::Albedo);
        shader.SetBool("useNormalTexture", true);
    } else {
        shader.SetBool("useNormalTexture", false);
    }
}

template <class Archive>
void
jleMaterial::serialize(Archive &ar)
{
    try {
        ar(CEREAL_NVP(_shaderRef));
    } catch (std::exception &e) {
        _shaderRef = jleResourceRef<jleShader>(jlePath{"ER:/shaders/defaultMesh.glsl"});
    }
    ar(CEREAL_NVP(_albedoTextureRef),
       CEREAL_NVP(_normalTextureRef),
       CEREAL_NVP(_metallicTextureRef),
       CEREAL_NVP(_roughnessTextureRef));
}
