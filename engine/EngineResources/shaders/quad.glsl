// Copyright (c) 2023. Johan Lind

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

/*BEGIN FRAG*/

uniform sampler2D texture0;

out vec4 FragColor;

in vec2 texCoords;
in vec4 uvs;		// Texture coords (x, y, width, height)
in vec2 texDims;

void main()
{
	vec4 col = texture(texture0, texCoords * vec2( uvs.z/texDims.x, uvs.w/texDims.y) + vec2(uvs.x/texDims.x,uvs.y/texDims.y));

	if(col.a < 0.1)
	discard;
	FragColor = col;
}