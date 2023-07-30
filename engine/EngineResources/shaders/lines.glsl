// Copyright (c) 2023. Johan Lind

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

/*BEGIN FRAG*/

out vec4 OutColor;

uniform vec3 cameraPos;

in vec3 pos;
in vec3 color;
in vec3 attenuation;

float CalculateAttenuation(float distance, float constant, float linear, float quadratic)
{
    return distance / (constant + linear * distance + quadratic * (distance*distance));
}

void main(){

    float distance = length(pos - cameraPos);
    float attenuationDistance = distance / 25.0;
    float attenuation = CalculateAttenuation(attenuationDistance, attenuation.x, attenuation.y, attenuation.z);

    OutColor = vec4(color, attenuation);
}