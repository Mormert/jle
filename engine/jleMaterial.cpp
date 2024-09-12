/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/
#include "jleMaterial.h"
#include "modules/graphics/core/jleIncludeGL.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleMaterial)
//JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleMaterialPBR)

void
jleMaterial::useMaterial(const jleCamera &camera,
                         const std::vector<jle3DRendererLight> &lights,
                         const jle3DSettings &settings,
                         jleSerializationContext &serializationContext)
{
    auto &shader = *_shaderRef.get();

    shader.use();
    shader.SetMat4("uView", camera.getViewMatrix());
    shader.SetMat4("uProj", camera.getProjectionMatrix());
    shader.SetVec3("uCameraPosition", camera.getPosition());
}

void
jleMaterial::setShader(const jleResourceRef<jleShader> &shaderRef)
{
    _shaderRef = shaderRef;
}

std::shared_ptr<jleShader>
jleMaterial::getShader()
{
    return _shaderRef.get();
}

std::shared_ptr<jleTexture>
jleMaterial::getOpacityTexture()
{
    return nullptr;
}

bool
jleMaterial::isTranslucent()
{
    return false;
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
                            const jle3DSettings &settings,
                            jleSerializationContext &serializationContext)
{
    // Temporary solution, loading the default shader here, before I figure something better out
    if (!_shaderRef) {
        _shaderRef = jleResourceRef<jleShader>(jlePath{"ER:/shaders/defaultMesh.glsl"}, serializationContext);
    }

    auto &shader = *_shaderRef.get();

    // Limit to 4 lights
    int lightCount = lights.size();
    if (lightCount > 4) {
        lightCount = 4;
    }

    shader.use();
    shader.SetTextureSlot("uShadowMap", jleTextureSlot::DirectionalShadow);
    shader.SetTextureSlot("uShadowMapPoint", jleTextureSlot::PointShadow);
    shader.SetTextureSlot("uAlbedoTexture", jleTextureSlot::Albedo);
    shader.SetTextureSlot("uNormalTexture", jleTextureSlot::Normal);
    shader.SetTextureSlot("uMetallicTexture", jleTextureSlot::Metallic);
    shader.SetTextureSlot("uRoughnessTexture", jleTextureSlot::Roughness);
    shader.SetTextureSlot("uOpacityTexture", jleTextureSlot::Opacity);
    shader.SetTextureSlot("uSkyboxTexture", jleTextureSlot::Skybox);
    shader.SetFloat("uFarPlane", 500.f);
    shader.SetBool("uUseDirectionalLight", settings.useDirectionalLight);
    shader.SetBool("uUseEnvironmentMapping", settings.useEnvironmentMapping && _useSkyboxEnvironmentMap);
    shader.SetVec3("uDirectionalLightColour", settings.directionalLightColour);
    shader.SetVec3("uDirectionalLightDir", settings.directionalLightRotation);
    shader.SetMat4("uView", camera.getViewMatrix());
    shader.SetMat4("uProj", camera.getProjectionMatrix());
    shader.SetMat4("uLightSpaceMatrix", settings.getLightSpaceMatrixAtPosition(camera.getPosition()));
    shader.SetVec3("uCameraPosition", camera.getPosition());
    shader.SetInt("uLightsCount", lightCount);

    for (int l = 0; l < lightCount; l++) {
        shader.SetVec3("uLightPositions[" + std::to_string(l) + "]", lights[l].position);
        shader.SetVec3("uLightColors[" + std::to_string(l) + "]", lights[l].color);
    }

    if (_albedo.textureRef()) {
        _albedo.textureRef()->setActive(jleTextureSlot::Albedo);
        shader.SetBool("uUseAlbedoTexture", true);
    } else {
        shader.SetBool("uUseAlbedoTexture", false);
        shader.SetVec3("uAlbedo", _albedo.color());
    }

    if (_normal.textureRef()) {
        _normal.textureRef()->setActive(jleTextureSlot::Normal);
        shader.SetBool("uUseNormalTexture", true);
    } else {
        shader.SetBool("uUseNormalTexture", false);
        shader.SetVec3("uNormal", _normal.color());
    }

    if (_metallic.textureRef()) {
        _metallic.textureRef()->setActive(jleTextureSlot::Metallic);
        shader.SetBool("uUseMetallicTexture", true);
    } else {
        shader.SetBool("uUseMetallicTexture", false);
        shader.SetFloat("uMetallic", _metallic.alpha());
    }

    if (_roughness.textureRef()) {
        _roughness.textureRef()->setActive(jleTextureSlot::Roughness);
        shader.SetBool("uUseRoughnessTexture", true);
    } else {
        shader.SetBool("uUseRoughnessTexture", false);
        shader.SetFloat("uRoughness", _roughness.alpha());
    }

    if (auto opacity = _opacity.textureRef()) {
        opacity->setActive(jleTextureSlot::Opacity);
        if (_singleChannelOpacity) {
            shader.SetBool("uOpacityTextureOneChannel", true);
        } else {
            shader.SetBool("uOpacityTextureOneChannel", false);
        }
        shader.SetBool("uUseOpacityTexture", true);
    } else {
        shader.SetBool("uUseOpacityTexture", false);
        shader.SetFloat("uOpacity", _opacity.alpha());
    }

    if (_blendModeDst == jleBlendMode::DISABLED || _blendModeSrc == jleBlendMode::DISABLED) {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc(static_cast<int>(_blendModeSrc) + 0x0300 - 1, static_cast<int>(_blendModeDst) + 0x0300 - 1);
    }
}

std::shared_ptr<jleTexture>
jleMaterialPBR::getOpacityTexture()
{
    if (_opacity.textureRef()) {
        return _opacity.textureRef().get();
    }
    return _albedo.textureRef().get();
}

bool
jleMaterialPBR::isTranslucent()
{
    return _isTranslucent;
}

jleMaterialPBR::jleMaterialPBR() {}

