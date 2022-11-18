#version 300 es

precision highp float;

out vec4 FragColor;

in vec3 Color;

void main()
{
    FragColor = vec4(Color, 1.0);
    FragColor.r = (Color.x+1.0)*0.5;
}