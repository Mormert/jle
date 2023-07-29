// Copyright (c) 2023. Johan Lind

#ifndef JLE_SHADER_H
#define JLE_SHADER_H

#include "jleSerializedResource.h"
#include "jleTypeReflectionUtils.h"

#include <cereal/cereal.hpp>

#include <glm/glm.hpp>

#include <string>

class jleShader : public jleSerializedResource, public std::enable_shared_from_this<jleShader>
{
public:
    unsigned int ID;

    JLE_REGISTER_RESOURCE_TYPE(jleShader, sh)

    template <class Archive>
    void
    save(Archive &ar) const
    {
        ar(CEREAL_NVP(_vertexPath), CEREAL_NVP(_fragPath));
    }

    template <class Archive>
    void
    load(Archive &ar)
    {
        jlePath beforeVertex = _vertexPath;
        jlePath beforeFrag = _fragPath;

        ar(CEREAL_NVP(_vertexPath), CEREAL_NVP(_fragPath));

        if(beforeVertex == _vertexPath && beforeFrag == _fragPath)
        {
            return;
        }

        if(!_vertexPath.isEmpty() && !_fragPath.isEmpty())
        {
            CreateFromSources(_vertexPath.getRealPath().c_str(), _fragPath.getRealPath().c_str());
        }
    }

    std::vector<std::string> getFileAssociationList() override;

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedResource)

    jleShader() = default;

    // Create shader from file
    jleShader(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr);

    ~jleShader() override;

    void use();

    void SetBool(const std::string &name, bool value) const;

    void SetInt(const std::string &name, int value) const;

    void SetFloat(const std::string &name, float value) const;

    void SetVec2(const std::string &name, const glm::vec2 &value) const;

    void SetVec2(const std::string &name, float x, float y) const;

    void SetVec3(const std::string &name, const glm::vec3 &value) const;

    void SetVec3(const std::string &name, float x, float y, float z) const;

    void SetVec4(const std::string &name, const glm::vec4 &value) const;

    void SetVec4(const std::string &name, float x, float y, float z, float w);

    void SetMat2(const std::string &name, const glm::mat2 &mat) const;

    void SetMat3(const std::string &name, const glm::mat3 &mat) const;

    void SetMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    void CreateFromSources(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr);

    void checkCompileErrors(unsigned int shader, std::string type);

    jlePath _vertexPath;
    jlePath _fragPath;
};

CEREAL_REGISTER_TYPE(jleShader)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedResource, jleShader)

namespace cereal
{
// This struct specialization will tell cereal to use the load/save pair, which we need to do since we're inheriting
// from a class that has serialize()
template <class Archive>
struct specialize<Archive, jleShader, cereal::specialization::member_load_save> {
};
} // namespace cereal

#endif // JLE_SHADER_H