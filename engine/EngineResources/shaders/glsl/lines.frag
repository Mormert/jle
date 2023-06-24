
out vec4 OutColor;

uniform vec3 color;
uniform vec3 cameraPos;
uniform vec3 attenuationParams;

in vec3 pos;

float CalculateAttenuation(float distance, float constant, float linear, float quadratic)
{
    return distance / (constant + linear * distance + quadratic * (distance*distance));
}

void main(){

    float distance = length(pos - cameraPos);
    float attenuationDistance = distance / 25.0;
    float attenuation = CalculateAttenuation(attenuationDistance, attenuationParams.x, attenuationParams.y, attenuationParams.z);

    OutColor = vec4(color, attenuation);
}