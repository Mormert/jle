#version 300 es

precision highp float;

out vec4 OutColor;

uniform vec4 PickingColor;

void main(){
    OutColor = PickingColor;
}