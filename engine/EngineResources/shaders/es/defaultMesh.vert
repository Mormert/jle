#version 300 es

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 projView;

void main()
{
    TexCoords = aTexCoords;

    // Make mat3 of the model matrix to remove translation on vector normals
    Normal = mat3(model) * aNormal;

    WorldPos = vec3(model * vec4(aPos, 1.0));

    gl_Position = projView * model * vec4(aPos, 1.0f);
}