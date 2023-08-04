// Copyright (c) 2023. Johan Lind

#ifndef JLE_SHADER_H
#define JLE_SHADER_H

#include "jleResourceInterface.h"
#include "jleTypeReflectionUtils.h"
#include "jleTextureRules.h"

#include <glm/glm.hpp>
#include <string>

class jleShader : public jleResourceInterface, public std::enable_shared_from_this<jleShader>
{
public:

    JLE_REGISTER_RESOURCE_TYPE(jleShader, glsl)

    jleLoadFromFileSuccessCode loadFromFile(const jlePath &path) override;

    std::vector<std::string> getFileAssociationList() override;

    jleShader() = default;

    ~jleShader() override;

    void use();

    void SetBool(const std::string &name, bool value) const;

    void SetInt(const std::string &name, int value) const;

    void SetTextureSlot(const std::string &name, jleTextureSlot slot) const;

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

    unsigned int _program{};

    bool checkCompileErrors(unsigned int shader, std::string type);
};


#endif // JLE_SHADER_H