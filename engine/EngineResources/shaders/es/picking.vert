#version 300 es

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projView;

void main()
{
    gl_Position = projView * model * vec4(aPos, 1.0f);
}