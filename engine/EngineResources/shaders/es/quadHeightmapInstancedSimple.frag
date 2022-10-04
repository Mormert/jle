#version 300 es

precision highp float;

uniform sampler2D texture_albedo;
uniform sampler2D texture_heightmap;
uniform sampler2D texture_normal;

uniform float sinZ;
uniform float sinZ_inverse;
uniform float magicHeightFactor;
uniform mat3 cartToIso;
uniform vec3 viewPos;

out vec4 FragColor;

in vec2 texCoords;
in vec4 uvs;// Texture coords (x, y, width, height)
in vec2 texDims;
in vec3 pos;

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

    FragColor = vec4(col.rgb, 1.0);
}