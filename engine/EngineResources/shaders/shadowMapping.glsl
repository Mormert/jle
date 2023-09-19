// Copyright (c) 2023. Johan Lind


layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

layout (location = 5) in ivec4 aBoneIds;
layout (location = 6) in vec4 aBoneWeights;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec2 TexCoords;

uniform bool uUseSkinning;
uniform mat4 uAnimBonesMatrices[100];

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
    gl_Position = lightSpaceMatrix * model * totalPosition;
}

/*BEGIN FRAG*/

#define BIAS 0.1

in vec2 TexCoords;

uniform bool uUseOpacityTexture;
uniform sampler2D uOpacityTexture;

void main()
{
    // Not doing anything in this frag shader is the equivalent to:
    // gl_FragDepth =  gl_FragCoord.z;
    // Note that using the above mentioned method, shadow acne will appear,
    // and needs to be accounted for in the mesh rendering shader.
    // Another way is to account for peter panning in this shadow mapping shader:

    // Account for vegetation, etc
    if (uUseOpacityTexture)
    {
        if (texture(uOpacityTexture, TexCoords).a < 0.1) {
            discard;
        }
    }

    gl_FragDepth = gl_FragCoord.z;
    gl_FragDepth += gl_FrontFacing ? BIAS : 0.0;// Mitigate shadow acne

    // The bias could also be calculated depending on surface normal and light direction
    // float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);
}