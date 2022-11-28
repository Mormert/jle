#version 330 core

out vec4 OutColor;

uniform vec4 PickingColor;

void main(){
    OutColor = PickingColor;
}