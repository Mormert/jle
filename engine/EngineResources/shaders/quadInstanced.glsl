// Copyright (c) 2023. Johan Lind


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

mat4 BuildTranslation(vec3 delta)
{
    return mat4(
    vec4(1.0, 0.0, 0.0, 0.0),
    vec4(0.0, 1.0, 0.0, 0.0),
    vec4(0.0, 0.0, 1.0, 0.0),
    vec4(delta, 1.0));
}

void main()
{
    mat4 model = BuildTranslation(vec3(position.xy, 0.0));
    gl_Position = camera * model * vec4(aPos * uv.zw, position.z, 1.0);

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