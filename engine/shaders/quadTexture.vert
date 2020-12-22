#version 330 core

uniform mat4 camera;

uniform vec2 textureDims;

uniform vec3 position; // world position (x, y, depth)
uniform vec4 uv; // Coords on texture (x, y, width, height)


layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

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