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

#ifndef JLE_MATERIAL_H
#define JLE_MATERIAL_H

#include "core/jleCommon.h"

#include "core/jleCompileHelper.h"
#include "jleResourceInterface.h"
#include "jleTypeReflectionUtils.h"

#include "jleCamera.h"
#include "jleResourceRef.h"
#include "jleShader.h"
#include "jleTexture.h"
#include "jleTextureRefOrRGBA.h"
#include "modules/graphics/jle3DSettings.h"
#include "modules/graphics/jleFramePacket.h"

#include "editor/jleImGuiArchive.h"
#include "serialization/jleBinaryArchive.h"
#include "serialization/jleJSONArchive.h"
#include <cereal/cereal.hpp>

enum class jleBlendMode : int32_t {
    DISABLED,
    SRC_COLOR,
    ONE_MINUS_SRC_COLOR,
    SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA,
    DST_ALPHA,
    ONE_MINUS_DST_ALPHA,
    DST_COLOR,
    ONE_MINUS_DST_COLOR,
    SRC_ALPHA_SATURATE
};

class jleMaterial : public jleSerializedOnlyResource
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleMaterial, "mat")

    ~jleMaterial() override = default;

    jleMaterial() = default;

    virtual void useMaterial(const jleCamera &camera,
                             const std::vector<jle3DRendererLight> &lights,
                             const jle3DSettings &settings);

    template <class Archive>
    void serialize(Archive &ar);

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedOnlyResource)

    void setShader(const jleResourceRef<jleShader> &shaderRef);

    std::shared_ptr<jleShader> getShader();

    virtual std::shared_ptr<jleTexture> getOpacityTexture();

    virtual bool isTranslucent();

protected:
    jleResourceRef<jleShader> _shaderRef;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleMaterial)

CEREAL_REGISTER_TYPE(jleMaterial)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleMaterial)

class jleMaterialPBR : public jleMaterial
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleMaterialPBR, "mat")

    jleMaterialPBR();

    void useMaterial(const jleCamera &camera,
                     const std::vector<jle3DRendererLight> &lights,
                     const jle3DSettings &settings) override;

    template <class Archive>
    void
    serialize(Archive &ar);

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleMaterial)

    std::shared_ptr<jleTexture> getOpacityTexture() override;

    bool isTranslucent() override;

    jleTextureRefOrRGB _albedo{glm::vec3{1.f}};           // White by default
    jleTextureRefOrRGB _normal{glm::vec3{0.f, 0.f, 1.f}}; // Normals pointing up by default
    jleTextureRefOrAlpha _metallic{0.0f};                 // No metallic by default
    jleTextureRefOrAlpha _roughness{0.5f};                // Half roughness by default
    jleTextureRefOrAlpha _opacity{1.0f};                  // Opaque by default
    bool _usePointShadows{true};
    bool _useDirectionalShadows{true};
    bool _useSkyboxEnvironmentMap{false};
    bool _isTranslucent{false};
    bool _singleChannelOpacity{false};
    jleBlendMode _blendModeSrc{};
    jleBlendMode _blendModeDst{};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleMaterialPBR)

CEREAL_REGISTER_TYPE(jleMaterialPBR)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleMaterial, jleMaterialPBR)

#endif // JLE_MATERIAL_H