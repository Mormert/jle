// Copyright (c) 2023. Johan Lind


layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

layout (location = 5) in ivec4 aBoneIds;
layout (location = 6) in vec4 aBoneWeights;


uniform mat4 lightSpaceMatrix;
uniform mat4 model;

uniform bool uUseSkinning;
uniform mat4 uAnimBonesMatrices[100];

out vec2 TexCoords;
out vec3 WorldPos;

void main()
{

    vec4 totalPosition = vec4(0.0);

    if(uUseSkinning){
        for(int i = 0; i < 4; ++i){
            if(aBoneIds[i] == -1){
                continue;
            }
            if(aBoneIds[i] >= 100){
                totalPosition = vec4(aPos, 1.0);
                break;
            }
            vec4 localPosition = uAnimBonesMatrices[aBoneIds[i]] * vec4(aPos, 1.0);
            totalPosition += localPosition * aBoneWeights[i];
        }
    }else{
        totalPosition = vec4(aPos, 1.0f);
    }

    TexCoords = aTexCoords;
    WorldPos = vec3(model * totalPosition);
    gl_Position = lightSpaceMatrix * model * totalPosition;
}

/*BEGIN FRAG*/

uniform float farPlane;
uniform vec3 lightPos;

in vec2 TexCoords;

in vec3 WorldPos;

uniform bool uUseOpacityTexture;
uniform sampler2D uOpacityTexture;

void main()
{
    // Account for vegetation, etc
    if (uUseOpacityTexture)
    {
        if (texture(uOpacityTexture, TexCoords).a < 0.1) {
            discard;
        }
    }

    float lightDistance = length(WorldPos - lightPos);

    // Linearly map the distance from 0 to 1
    lightDistance = lightDistance / farPlane;

    // Manually set the depth buffer value
    gl_FragDepth = lightDistance;
}