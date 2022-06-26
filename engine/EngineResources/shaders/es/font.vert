#version 300 es
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 projection;
uniform float depth;

void main()
{
    gl_Position = projection * vec4(vertex.xy, depth, 1.0);
    TexCoords = vertex.zw;
}