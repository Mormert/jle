#version 300 es

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 WorldFragPos;
    vec4 WorldFragPosLightSpace;
    vec3 TangentFragPos;
    vec3 TangentLightPos[4];
    vec3 TangentCameraPos;
    vec2 TexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix;
uniform vec3 LightPositions[4];
uniform vec3 CameraPosition;

void main()
{
    vs_out.TexCoords = aTexCoords;

    // Gram-Schmidt Orthogonalisation
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.WorldFragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TangentFragPos = TBN * vs_out.WorldFragPos;
    for(int i = 0; i < 4; i++)
    {
        vs_out.TangentLightPos[i] = TBN * LightPositions[i];
    }
    vs_out.TangentCameraPos = TBN * CameraPosition;
    vs_out.WorldFragPosLightSpace = lightSpaceMatrix * vec4(vs_out.WorldFragPos, 1.0);

    gl_Position = proj * view * model * vec4(aPos, 1.0f);
}