// Copyright (c) 2023. Johan Lind

layout (location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 projection;
uniform float depth;

void main()
{
    gl_Position = projection * vec4(vertex.xy, depth, 1.0);
    TexCoords = vertex.zw;
}

/*BEGIN FRAG*/

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).a); // Read alpha value only on GL ES
    color = vec4(textColor, 1.0) * sampled;
}