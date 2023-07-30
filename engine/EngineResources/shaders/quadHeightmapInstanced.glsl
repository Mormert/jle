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
out vec3 pos;

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
    pos = position;
}

/*BEGIN FRAG*/

uniform sampler2D texture_albedo;
uniform sampler2D texture_heightmap;
uniform sampler2D texture_normal;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;


uniform float sinZ;
uniform float sinZ_inverse;
uniform float magicHeightFactor;
uniform mat3 cartToIso;
uniform vec3 viewPos;

uniform bool gamma;

out vec4 FragColor;

in vec2 texCoords;
in vec4 uvs;// Texture coords (x, y, width, height)
in vec2 texDims;
in vec3 pos;

// Just a generic Blinn-Phong with simple attenuation that is gamma corrected
vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
    // diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // simple attenuation
    float distance = length(lightPos - fragPos) * 0.0075;
    float attenuation = 1.0 / (gamma ? distance * distance : distance);

    diffuse *= attenuation;
    specular *= attenuation;

    return diffuse + specular;
}

void main()
{
    // Calculate the actual pixel coordinates on the texture
    vec2 texture_coordinate = texCoords * vec2(uvs.z/texDims.x, uvs.w/texDims.y) + vec2(uvs.x/texDims.x, uvs.y/texDims.y);
    vec4 col = vec4(light.ambient, 1.0) * texture(texture_albedo, texture_coordinate.xy);

    if (col.a < 0.01)
    {
        discard;
    }

    // Sample the height value, ranging from 0 to 255 from the height map
    float height = texture(texture_heightmap, texture_coordinate.xy).r;

    // Calculate the original position for the fragment (the pixel)
    vec3 fragPos3D = vec3(pos.xy - uvs.xy + texture_coordinate*texDims, 0.0);

    // We need to adjust the height with a "magic height factor", that is some value
    // that depends on the amount of pixels that span from zero height value to maximum height value.
    float heightPixel = height * magicHeightFactor;

    // Add the height from the y, taking account the camera z angle
    fragPos3D.y += heightPixel * sinZ;

    // Override depth buffer value with heightmap value (multiply with 0.5 because it seems to clip elsewise)
    gl_FragDepth = gl_FragCoord.z - height * 0.50;

    // Transform the XY position to the isometric coordinates, and set the Z value
    fragPos3D = vec3(fragPos3D.xy, 0.0)*cartToIso + vec3(0.0, 0.0, heightPixel);

    // Calculate the lights position in isometric coordinates
    vec3 lightPosIso = vec3(light.position.x, light.position.y * sinZ_inverse, light.position.z);

    // Get the normals in local space
    vec3 normal = 2.0*texture(texture_normal, texture_coordinate.xy).rgb-1.0;

    vec3 lightning = BlinnPhong(normal, fragPos3D, lightPosIso, vec3(1.0));
    lightning = clamp(lightning, vec3(0.2), vec3(1.0));
    col.rgb *= lightning;
    if (gamma)
    {
        col.rgb = pow(col.rgb, vec3(1.0/2.2));
    }
    FragColor = vec4(col.rgb, 1.0);
}