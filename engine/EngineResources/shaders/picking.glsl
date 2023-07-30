// Copyright (c) 2023. Johan Lind

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projView;

void main()
{
    gl_Position = projView * model * vec4(aPos, 1.0f);
}

/*BEGIN FRAG*/

out vec4 OutColor;

uniform vec4 PickingColor;

void main(){
    OutColor = PickingColor;
}