// Copyright (c) 2022. Johan Lind

#pragma once

#include <glm/glm.hpp>

#include <string>

namespace jle {
    namespace gfx {
        class Shader_OpenGL {
        public:
            unsigned int ID;

            // Create shader from file
            Shader_OpenGL(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr);

            // Create shader from string containing the shader code
            //explicit Shader_OpenGL(std::string vertexCode, std::string fragmentCode, std::string geometryCode = "");

            ~Shader_OpenGL();

            void Use();

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
            void CheckCompileErrors(unsigned int shader, std::string type);

            std::string vertexCode;
            std::string fragCode;
        };
    }
}