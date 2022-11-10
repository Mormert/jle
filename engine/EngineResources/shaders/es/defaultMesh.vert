#version 300 es

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;

out vec3 Color;

uniform mat4 model;
uniform mat4 projView;

void main()
{
    Color = aNormal;
    gl_Position = projView * model * vec4(aPos, 1.0f);
}