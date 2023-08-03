// Copyright (c) 2023. Johan Lind

#include "jleShader.h"
#include "plog/Log.h"

#include "jleIncludeGL.h"

#include <fstream>
#include <string>

jleLoadFromFileSuccessCode
jleShader::loadFromFile(const jlePath &path)
{
    std::ifstream load{path.getRealPath()};
    if (!load.good()) {
        return jleLoadFromFileSuccessCode::FAIL;
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

#ifdef BUILD_OPENGLES30
    vert = "#version 300 es\n" + vert;
    frag = "#version 300 es\nprecision highp float;\n" + frag;
#else
    vert = "#version 330 core\n" + vert;
    frag = "#version 330 core\n" + frag;
#endif

    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    const char *vert_c = vert.c_str();
    glShaderSource(vertex, 1, &vert_c, NULL);
    glCompileShader(vertex);
    if (!checkCompileErrors(vertex, "VERTEX")) {
        glDeleteShader(vertex);
        return jleLoadFromFileSuccessCode::FAIL;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char *frag_c = frag.c_str();
    glShaderSource(fragment, 1, &frag_c, NULL);
    glCompileShader(fragment);
    if (!checkCompileErrors(fragment, "FRAGMENT")) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return jleLoadFromFileSuccessCode::FAIL;
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
        return jleLoadFromFileSuccessCode::FAIL;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    LOGV << "Compiled GLSL shader program: " << path.getVirtualPath();

    return jleLoadFromFileSuccessCode::SUCCESS;

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
jleShader::SetInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(_program, name.c_str()), value);
}

void
jleShader::SetFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(_program, name.c_str()), value);
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
jleShader::SetVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(_program, name.c_str()), 1, &value[0]);
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
            LOG_ERROR << "Error in shader: " << filepath << ", of type: " << type << ": " << infoLog;
            return false;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            LOG_ERROR << "Error linking shader: " << filepath << ", of type: " << type << ": " << infoLog;
            return false;
        }
    }
    return true;
}
std::vector<std::string>
jleShader::getFileAssociationList()
{
    return {"glsl"};
}
