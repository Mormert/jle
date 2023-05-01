#version 300 es

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 WorldFragPos;
out vec4 WorldFragPosLightSpace;
out vec3 WorldCameraPos;
out vec3 TangentFragPos;
out vec3 TangentLightPos[4];
out vec3 TangentCameraPos;
out vec2 TexCoords;
out vec3 localNormal;

out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;
uniform vec3 LightPositions[4];
uniform vec3 CameraPosition;

void main()
{
    TexCoords = aTexCoords;

    // Gram-Schmidt Orthogonalisation
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    localNormal = normalize(aNormal);
    TBN = transpose(mat3(T, B, N));

    WorldFragPos = vec3(model * vec4(aPos, 1.0));
    TangentFragPos = TBN * WorldFragPos;
    for(int i = 0; i < 4; i++)
    {
        TangentLightPos[i] = TBN * LightPositions[i];
    }
    TangentCameraPos = TBN * CameraPosition;
    WorldCameraPos = CameraPosition;
    WorldFragPosLightSpace = lightSpaceMatrix * vec4(WorldFragPos, 1.0);

    gl_Position = proj * view * model * vec4(aPos, 1.0f);
}