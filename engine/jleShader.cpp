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

#include "jleShader.h"
#include "jleRenderThread.h"
#include "modules/graphics/core/jleIncludeGL.h"

#include <plog/Log.h>

#include <fstream>
#include <string>

bool
jleShader::loadFromFile(jleSerializationContext &ctx, const jlePath &path)
{
    JLE_EXEC_IF(JLE_BUILD_HEADLESS) { return true; }

    std::ifstream load{path.getRealPath()};
    if (!load.good()) {
        return false;
    }

    std::stringstream buffer;
    buffer << load.rdbuf();

    auto shaderSource = buffer.str();

    unsigned first = 0;
    unsigned last = shaderSource.find("/*BEGIN FRAG*/");
    auto vert = shaderSource.substr(first, last - first);

    first = last;
    last = shaderSource.back();
    auto frag = shaderSource.substr(first, last - first);

    JLE_EXEC_IF(JLE_BUILD_OPENGLES30)
    {
        vert = "#version 300 es\n" + vert;
        frag = "#version 300 es\nprecision highp float;\n" + frag;
    }
    else
    {
        vert = "#version 330 core\n" + vert;
        frag = "#version 330 core\n" + frag;
    }

    jleAssert(ctx.renderThread);
    if (ctx.renderThread) {
        ctx.renderThread->runOnRenderThread([this, vert, frag, path]() {
            unsigned int vertex, fragment;

            vertex = glCreateShader(GL_VERTEX_SHADER);
            const char *vert_c = vert.c_str();
            glShaderSource(vertex, 1, &vert_c, NULL);
            glCompileShader(vertex);
            if (!checkCompileErrors(vertex, "VERTEX")) {
                glDeleteShader(vertex);
                return;
            }

            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            const char *frag_c = frag.c_str();
            glShaderSource(fragment, 1, &frag_c, NULL);
            glCompileShader(fragment);
            if (!checkCompileErrors(fragment, "FRAGMENT")) {
                glDeleteShader(vertex);
                glDeleteShader(fragment);
                return;
            }

            _program = glCreateProgram();
            glAttachShader(_program, vertex);
            glAttachShader(_program, fragment);

            glLinkProgram(_program);
            if (!checkCompileErrors(_program, "PROGRAM")) {
                glDeleteShader(vertex);
                glDeleteShader(fragment);
                glDeleteProgram(_program);
                _program = 0;
                return;
            }

            glDeleteShader(vertex);
            glDeleteShader(fragment);

            LOGV << "Compiled GLSL shader program: " << path.getVirtualPath();
        });
    }

    return true;
}

jleShader::~jleShader() { glDeleteProgram(_program); }

void
jleShader::use()
{
    glUseProgram(_program);
}

void
jleShader::SetBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(_program, name.c_str()), (int)value);
}

void
jleShader::SetBool(const std::string &name, const std::vector<bool> &uniformArray) const
{
    std::vector<int> intConversion;
    intConversion.resize(uniformArray.size());
    for (const auto &b : uniformArray) {
        intConversion.push_back((int)b);
    }
    glUniform1iv(glGetUniformLocation(_program, name.c_str()), uniformArray.size(), &intConversion[0]);
}

void
jleShader::SetInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(_program, name.c_str()), value);
}

void
jleShader::SetInt(const std::string &name, const std::vector<int> &uniformArray) const
{
    glUniform1iv(glGetUniformLocation(_program, name.c_str()), uniformArray.size(), &uniformArray[0]);
}

void
jleShader::SetTextureSlot(const std::string &name, jleTextureSlot slot) const
{
    glUniform1i(glGetUniformLocation(_program, name.c_str()), static_cast<int>(slot));
}

void
jleShader::SetFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(_program, name.c_str()), value);
}

void
jleShader::SetFloat(const std::string &name, const std::vector<float> &uniformArray) const
{
    glUniform1fv(glGetUniformLocation(_program, name.c_str()), uniformArray.size(), &uniformArray[0]);
}

void
jleShader::SetVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(_program, name.c_str()), 1, &value[0]);
}

void
jleShader::SetVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(_program, name.c_str()), x, y);
}

void
jleShader::SetVec2(const std::string &name, const std::vector<glm::vec2> &uniformArray) const
{
    glUniform2fv(glGetUniformLocation(_program, name.c_str()), uniformArray.size(), &uniformArray[0].x);
}

void
jleShader::SetVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(_program, name.c_str()), 1, &value[0]);
}

void
jleShader::SetVec3(const std::string &name, const std::vector<glm::vec3> &uniformArray) const
{
    glUniform3fv(glGetUniformLocation(_program, name.c_str()), uniformArray.size(), &uniformArray[0].x);
}

void
jleShader::SetVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(_program, name.c_str()), x, y, z);
}

void
jleShader::SetVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(_program, name.c_str()), 1, &value[0]);
}

void
jleShader::SetVec4(const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(_program, name.c_str()), x, y, z, w);
}

void
jleShader::SetVec4(const std::string &name, const std::vector<glm::vec4> &uniformArray) const
{
    glUniform4fv(glGetUniformLocation(_program, name.c_str()), uniformArray.size(), &uniformArray[0].x);
}

void
jleShader::SetMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void
jleShader::SetMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void
jleShader::SetMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

bool
jleShader::checkCompileErrors(unsigned int shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            LOG_ERROR << "Error in shader: " << path.getVirtualPath() << ", of type: " << type << ": " << infoLog;
            return false;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            LOG_ERROR << "Error linking shader: " << path.getVirtualPath() << ", of type: " << type << ": " << infoLog;
            return false;
        }
    }
    return true;
}
