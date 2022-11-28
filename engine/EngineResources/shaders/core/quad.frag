#version 330 core

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