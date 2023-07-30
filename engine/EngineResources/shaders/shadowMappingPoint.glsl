// Copyright (c) 2023. Johan Lind


layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec3 WorldPos;

void main()
{
    WorldPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}

/*BEGIN FRAG*/

uniform float farPlane;
uniform vec3 lightPos;

in vec3 WorldPos;

void main()
{
    float lightDistance = length(WorldPos - lightPos);

    // Linearly map the distance from 0 to 1
    lightDistance = lightDistance / farPlane;

    // Manually set the depth buffer value
    gl_FragDepth = lightDistance;
}