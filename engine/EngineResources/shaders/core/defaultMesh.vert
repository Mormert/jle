#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

out vec3 AnisotropicAxis;
out vec3 CentralAxis;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;

void main()
{
    TexCoords = aTexCoords;

    // Make mat3 of the model matrix to remove translation on vector normals
    Normal = mat3(model) * aNormal;

    WorldPos = vec3(model * vec4(aPos, 1.0));
    FragPosLightSpace = lightSpaceMatrix * vec4(WorldPos, 1.0);

    AnisotropicAxis = vec3(model * vec4(0.0, 0.0, 1.0, 0.0));
    CentralAxis = vec3(model * vec4(aPos.xy, 0.0, 0.0));


    gl_Position = proj * view * model * vec4(aPos, 1.0f);
}