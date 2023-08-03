// Copyright (c) 2023. Johan Lind

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in mat4 aInstanceMatrix;

out vec3 Color;

uniform mat4 projView;

void main()
{
    Color = aColor;
    gl_Position = projView * aInstanceMatrix * vec4(aPos, 1.0f);
}

/*BEGIN FRAG*/

out vec4 FragColor;

in vec3 Color;

void main()
{
    FragColor = vec4(Color, 1.0);
}