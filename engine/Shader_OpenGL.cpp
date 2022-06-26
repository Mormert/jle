// Copyright (c) 2022. Johan Lind

#include "Shader_OpenGL.h"
#include "plog/Log.h"
#include "jleStaticOpenGLState.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

#include <string>
#include <fstream>
#include <iostream>

// Inspired & based on examples found on learnopengl.com

namespace jle {
    namespace gfx {
        Shader_OpenGL::Shader_OpenGL(const char *vertexPath, const char *fragmentPath, const char *geometryPath) {

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
            catch (std::ifstream::failure &e) {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << e.what() << std::endl;
            }
            const char *vShaderCode = vertexCode.c_str();
            const char *fShaderCode = fragmentCode.c_str();

            unsigned int vertex, fragment;

            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glCompileShader(vertex);
            CheckCompileErrors(vertex, "VERTEX");

            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShaderCode, NULL);
            glCompileShader(fragment);
            CheckCompileErrors(fragment, "FRAGMENT");
            // if geometry shader is given, compile geometry shader - Not supported in OpenGL ES 3.0
            // unsigned int geometry;
            // if (geometryPath != nullptr)
            // {
            // 	const char* gShaderCode = geometryCode.c_str();
            // 	geometry = glCreateShader(GL_GEOMETRY_SHADER);
            // 	glShaderSource(geometry, 1, &gShaderCode, NULL);
            // 	glCompileShader(geometry);
            // 	CheckCompileErrors(geometry, "GEOMETRY");
            // }
            // shader Program
            ID = glCreateProgram();
            glAttachShader(ID, vertex);
            glAttachShader(ID, fragment);
            // if (geometryPath != nullptr)
            // 	glAttachShader(ID, geometry);
            glLinkProgram(ID);
            CheckCompileErrors(ID, "PROGRAM");
            // delete the shaders as they're linked into our program now and no longer necessery
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            // if (geometryPath != nullptr)
            // 	glDeleteShader(geometry);

            LOG_VERBOSE << "Compiled shader, ID: " << ID;

        }

        /*Shader_OpenGL::Shader_OpenGL(std::string vertexCode, std::string fragmentCode, std::string geometryCode)
        {
            const char* vShaderCode = vertexCode.c_str();
            const char* fShaderCode = fragmentCode.c_str();
            this->vertexCode = vertexCode;
            this->fragCode = fragmentCode;

            unsigned int vertex, fragment;

            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glCompileShader(vertex);
            CheckCompileErrors(vertex, "VERTEX");

            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShaderCode, NULL);
            glCompileShader(fragment);
            CheckCompileErrors(fragment, "FRAGMENT");
            // if geometry shader is given, compile geometry shader, geometry not supported in OpenGL ES 3.0
            // unsigned int geometry{ 0 };
            // if (geometryCode != "")
            // {
            // 	const char* gShaderCode = geometryCode.c_str();
            // 	geometry = glCreateShader(GL_GEOMETRY_SHADER);
            // 	glShaderSource(geometry, 1, &gShaderCode, NULL);
            // 	glCompileShader(geometry);
            // 	CheckCompileErrors(geometry, "GEOMETRY");
            // }
            // shader Program
            ID = glCreateProgram();
            glAttachShader(ID, vertex);
            glAttachShader(ID, fragment);
            // if (geometryCode != "")
            // 	glAttachShader(ID, geometry);
            glLinkProgram(ID);
            CheckCompileErrors(ID, "PROGRAM");
            // delete the shaders as they're linked into our program now and no longer necessery
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            // if (geometryCode != "")
            // 	glDeleteShader(geometry);
        }*/

        Shader_OpenGL::~Shader_OpenGL() {
            glDeleteProgram(ID);
        }

        void Shader_OpenGL::Use() {
            glUseProgram(ID);
        }

        void Shader_OpenGL::SetBool(const std::string &name, bool value) const {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
        }

        void Shader_OpenGL::SetInt(const std::string &name, int value) const {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        }

        void Shader_OpenGL::SetFloat(const std::string &name, float value) const {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        }

        void Shader_OpenGL::SetVec2(const std::string &name, const glm::vec2 &value) const {
            glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }

        void Shader_OpenGL::SetVec2(const std::string &name, float x, float y) const {
            glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
        }

        void Shader_OpenGL::SetVec3(const std::string &name, const glm::vec3 &value) const {
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }

        void Shader_OpenGL::SetVec3(const std::string &name, float x, float y, float z) const {
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
        }

        void Shader_OpenGL::SetVec4(const std::string &name, const glm::vec4 &value) const {
            glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }

        void Shader_OpenGL::SetVec4(const std::string &name, float x, float y, float z, float w) {
            glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
        }

        void Shader_OpenGL::SetMat2(const std::string &name, const glm::mat2 &mat) const {
            glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        void Shader_OpenGL::SetMat3(const std::string &name, const glm::mat3 &mat) const {
            glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        void Shader_OpenGL::SetMat4(const std::string &name, const glm::mat4 &mat) const {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }


        void Shader_OpenGL::CheckCompileErrors(unsigned int shader, std::string type) {
            GLint success;
            GLchar infoLog[1024];
            if (type != "PROGRAM") {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                    LOG_ERROR << "Error in shader: " << vertexCode << '\n' << fragCode << '\n';
                    std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
                              << "\n -- --------------------------------------------------- -- " << std::endl;
                }
            } else {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success) {
                    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                    LOG_ERROR << "Error in shader: " << vertexCode << '\n' << fragCode << '\n';
                    std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
                              << "\n -- --------------------------------------------------- -- " << std::endl;
                }
            }
        }
    }
}