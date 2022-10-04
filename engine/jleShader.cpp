// Copyright (c) 2022. Johan Lind

#include "jleShader.h"
#include "jleStaticOpenGLState.h"
#include "plog/Log.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

#include <fstream>
#include <iostream>
#include <string>

// Inspired & based on examples found on learnopengl.com

jleShader::jleShader(const char *vertexPath,
                     const char *fragmentPath,
                     const char *geometryPath) {

    assert(jleStaticOpenGLState::globalOpenGLInitialized == true);

    LOG_VERBOSE << "Compiling shader: " << vertexPath << " , " << fragmentPath;

    this->vertexCode = vertexPath;
    this->fragCode = fragmentPath;

    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {

        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if (geometryPath != nullptr) {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << e.what()
                  << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // if geometry shader is given, compile geometry shader - Not supported in
    // OpenGL ES 3.0 unsigned int geometry; if (geometryPath != nullptr)
    // {
    // 	const char* gShaderCode = geometryCode.c_str();
    // 	geometry = glCreateShader(GL_GEOMETRY_SHADER);
    // 	glShaderSource(geometry, 1, &gShaderCode, NULL);
    // 	glCompileShader(geometry);
    // 	checkCompileErrors(geometry, "GEOMETRY");
    // }
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    // if (geometryPath != nullptr)
    // 	glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer
    // necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    // if (geometryPath != nullptr)
    // 	glDeleteShader(geometry);

    LOG_VERBOSE << "Compiled shader, ID: " << ID;
}

/*Shader_OpenGL::Shader_OpenGL(std::string vertexCode, std::string fragmentCode,
std::string geometryCode)
{
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    this->vertexCode = vertexCode;
    this->fragCode = fragmentCode;

    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // if geometry shader is given, compile geometry shader, geometry not
supported in OpenGL ES 3.0
    // unsigned int geometry{ 0 };
    // if (geometryCode != "")
    // {
    // 	const char* gShaderCode = geometryCode.c_str();
    // 	geometry = glCreateShader(GL_GEOMETRY_SHADER);
    // 	glShaderSource(geometry, 1, &gShaderCode, NULL);
    // 	glCompileShader(geometry);
    // 	checkCompileErrors(geometry, "GEOMETRY");
    // }
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    // if (geometryCode != "")
    // 	glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer
necessery glDeleteShader(vertex); glDeleteShader(fragment);
    // if (geometryCode != "")
    // 	glDeleteShader(geometry);
}*/

jleShader::~jleShader() { glDeleteProgram(ID); }

void jleShader::Use() { glUseProgram(ID); }

void jleShader::SetBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void jleShader::SetInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void jleShader::SetFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void jleShader::SetVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void jleShader::SetVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void jleShader::SetVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void jleShader::SetVec3(const std::string& name,
                        float x,
                        float y,
                        float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void jleShader::SetVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void jleShader::SetVec4(
    const std::string& name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void jleShader::SetMat2(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(
        glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void jleShader::SetMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(
        glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void jleShader::SetMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(
        glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void jleShader::checkCompileErrors(unsigned int shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            LOG_ERROR << "Error in shader: " << vertexCode << '\n'
                      << fragCode << '\n';
            std::cout
                << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                << infoLog
                << "\n -- --------------------------------------------------- "
                   "-- "
                << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            LOG_ERROR << "Error in shader: " << vertexCode << '\n'
                      << fragCode << '\n';
            std::cout
                << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                << infoLog
                << "\n -- --------------------------------------------------- "
                   "-- "
                << std::endl;
        }
    }
}