#version 300 es

precision highp float;

out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform int LightsCount;
uniform vec3 LightPositions[4];
uniform vec3 LightColors[4];

uniform vec3 CameraPosition;


vec3 lambertian_brdf(vec3 in_direction, vec3 out_direction, vec3 normal)
{
    return vec3(0.5, 0.5, 0.5);
}

void main()
{

    vec3 N = normalize(Normal);
    vec3 V = normalize(CameraPosition - WorldPos);

    vec3 LightOutTotal = vec3(0.0);
    for (int l = 0; l < LightsCount; ++l)
    {
        vec3 L = normalize(LightPositions[l] - WorldPos);
        vec3 H = normalize(V + L);// Halfway-vector

        // Incident angle
        float NdotL = max(dot(N, L), 0.0);

        // Incoming radiance from the light source
        float distance = length(LightPositions[l] - WorldPos) * 0.001;
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = LightColors[l] * attenuation;

        LightOutTotal += radiance * lambertian_brdf(L, V, N) * NdotL;

    }

    FragColor = vec4(LightOutTotal, 1.0);
}