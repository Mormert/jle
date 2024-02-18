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

#ifndef JLE_SHADER_H
#define JLE_SHADER_H

#include "jleCommon.h"

#include "jleResourceInterface.h"
#include "jleTextureRules.h"
#include "jleTypeReflectionUtils.h"

#include <glm/glm.hpp>
#include <string>

class jleShader : public jleResourceInterface, public std::enable_shared_from_this<jleShader>
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleShader, "glsl")

    [[nodiscard]] bool loadFromFile(const jlePath &path) override;

    jleShader() = default;

    ~jleShader() override;

    void use();

    void SetBool(const std::string &name, bool value) const;

    void SetBool(const std::string &name, const std::vector<bool>& uniformArray) const;

    void SetInt(const std::string &name, int value) const;

    void SetInt(const std::string &name, const std::vector<int>& uniformArray) const;

    void SetTextureSlot(const std::string &name, jleTextureSlot slot) const;

    void SetFloat(const std::string &name, float value) const;

    void SetFloat(const std::string &name, const std::vector<float>& uniformArray) const;

    void SetVec2(const std::string &name, const glm::vec2 &value) const;

    void SetVec2(const std::string &name, float x, float y) const;

    void SetVec2(const std::string &name, const std::vector<glm::vec2> &uniformArray) const;

    void SetVec3(const std::string &name, const glm::vec3 &value) const;

    void SetVec3(const std::string &name, float x, float y, float z) const;

    void SetVec3(const std::string &name, const std::vector<glm::vec3> &uniformArray) const;

    void SetVec4(const std::string &name, const glm::vec4 &value) const;

    void SetVec4(const std::string &name, float x, float y, float z, float w);

    void SetVec4(const std::string &name, const std::vector<glm::vec4> &uniformArray) const;

    void SetMat2(const std::string &name, const glm::mat2 &mat) const;

    void SetMat3(const std::string &name, const glm::mat3 &mat) const;

    void SetMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    unsigned int _program{};

    bool checkCompileErrors(unsigned int shader, std::string type);
};

#endif // JLE_SHADER_H