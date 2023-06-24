
layout (location = 0) in vec3 aPosWorldSpace;

uniform mat4 projView;

out vec3 pos;

void main()
{
    gl_Position = projView * vec4(aPosWorldSpace, 1.0f);
    pos = aPosWorldSpace;
}