// Copyright (c) 2023. Johan Lind

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 Normal;

void main()
{
    Normal = aNormal;
    gl_Position = proj * view * model * vec4(aPos, 1.0f);
}

/*BEGIN FRAG*/

in vec3 Normal;

out vec4 FragColor;

void main()
{
    float c = dot(Normal, normalize(vec3(0.2, -1.0, 0.2)));
    c = c * 0.5 + 0.5;
    vec3 color = vec3(0.8, 0.3, 0.8) * c;
    FragColor = vec4(color, 1.0);
}












