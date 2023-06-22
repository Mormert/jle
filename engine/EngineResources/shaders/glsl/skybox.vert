
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);

    // Set the z component to w, such that after perspective division
    // it will equal 1, and therefore the maximum depth value.
    gl_Position = pos.xyww;
}