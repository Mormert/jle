#version 300 es

precision highp float;

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