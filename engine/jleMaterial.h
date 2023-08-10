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

    virtual std::shared_ptr<jleTexture> getOpacityTexture();

    virtual bool isTranslucent();

protected:
    jleResourceRef<jleShader> _shaderRef;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleMaterial)

CEREAL_REGISTER_TYPE(jleMaterial)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedResource, jleMaterial)

#endif // JLE_MATERIAL_H