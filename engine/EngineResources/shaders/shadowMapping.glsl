// Copyright (c) 2023. Johan Lind


layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
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