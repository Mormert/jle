#version 300 es
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

layout (location = 2) in vec3 position;		// world position		(x, y, depth)
layout (location = 3) in vec4 uv;			// Coords on texture	(tex_x, tex_y, tex_width, tex_height)

uniform mat4 camera;
uniform vec2 textureDims;

//out vec3 fColor;

out vec4 uvs;
out vec2 texCoords;
out vec2 texDims;

void main()
{
    gl_Position = camera * vec4(aPos * uv.zw + position.xy, position.z, 1.0);

    texCoords = aTexCoords;
    uvs = uv;
    texDims = textureDims;
}