#version 300 es
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec4 glPos;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
    glPos = gl_Position;
}