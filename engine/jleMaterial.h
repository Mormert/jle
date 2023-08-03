// Copyright (c) 2023. Johan Lind

#ifndef JLE_MATERIAL_H
#define JLE_MATERIAL_H

#include "jleResourceInterface.h"
#include "jleTypeReflectionUtils.h"

#include "jleResourceRef.h"
#include "jleTexture.h"
#include "jleShader.h"

#include <cereal/cereal.hpp>


class jleMaterial : public jleSerializedResource, public std::enable_shared_from_this<jleMaterial>
{
public:

    JLE_REGISTER_RESOURCE_TYPE(jleMaterial, mat)

    ~jleMaterial() override = default;

    jleMaterial() = default;


    virtual void useMaterial(/*const jle3DRendererSettings&*/);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        try{
            ar(CEREAL_NVP(_shaderRef));

        }catch(std::exception &e)
        {
            _shaderRef = jleResourceRef<jleShader>(jlePath{"ER:/shaders/defaultMesh.glsl"});
        }
        ar(CEREAL_NVP(_albedoTextureRef),
           CEREAL_NVP(_normalTextureRef),
           CEREAL_NVP(_metallicTextureRef),
           CEREAL_NVP(_roughnessTextureRef));
    }

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedResource)

    std::vector<std::string> getFileAssociationList() override;

    jleResourceRef<jleShader> _shaderRef;
    jleResourceRef<jleTexture> _albedoTextureRef;
    jleResourceRef<jleTexture> _normalTextureRef;
    jleResourceRef<jleTexture> _metallicTextureRef;
    jleResourceRef<jleTexture> _roughnessTextureRef;
};

CEREAL_REGISTER_TYPE(jleMaterial)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedResource, jleMaterial)

#endif // JLE_MATERIAL_H