
layout (location = 0) in vec3 aPosWorldSpace;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aAttenuation;

uniform mat4 projView;

out vec3 pos;
out vec3 color;
out vec3 attenuation;

void main()
{
    gl_Position = projView * vec4(aPosWorldSpace, 1.0f);
    pos = aPosWorldSpace;
    color = aColor;
    attenuation = aAttenuation;
}