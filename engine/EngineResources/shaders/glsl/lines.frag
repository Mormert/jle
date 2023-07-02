
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