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

const float pi = 3.141592653589;
const vec3 albedo = vec3(0.0, 0.2, 0.6);
const float metallic = 0.0;
const float roughness = 0.2;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (1.0 + NdotH2 * (a2 - 1.0));
    denom = pi * denom * denom;

    return nom / denom;
}

float DistributionBeckmann(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.00001);// add some margin to avoid divide-by-zero
    float NdotH2 = NdotH*NdotH;
    float NdotH4 = NdotH2*NdotH2;

    float nom = NdotH;
    float denom = pi * a2 * NdotH4;
    float nom_exp = NdotH2 - 1.0;
    float denom_exp = a2 * NdotH2;

    return (nom/denom) * exp(nom_exp/denom_exp);
}

float GeometryOriginalCookTorrance(vec3 N, vec3 V, vec3 L)
{
    vec3 H = normalize(V + L);
    float NdotH = max(dot(N, H), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float VdotH = max(dot(V, H), 0.0);

    float max1 = max((2.0*NdotH*NdotV)/(VdotH), (2.0*NdotH*NdotL)/(VdotH));
    return max(1.0, max1);

}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlickApprox(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 lambertian_brdf(vec3 in_direction, vec3 out_direction, vec3 normal)
{
    float NdotL = dot(normal, in_direction);
    return max(0.0, NdotL) * albedo;
}

// https://www.shadertoy.com/view/ldBGz3
// https://garykeen27.wixsite.com/portfolio/oren-nayar-shading
float oren_nayar_brdf(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal) {
    float r2 = roughness*roughness;
    float a = 1.0 - 0.5*(r2/(r2+0.57));
    float b = 0.45*(r2/(r2+0.09));

    float nl = dot(surfaceNormal, lightDirection);
    float nv = dot(surfaceNormal, viewDirection);

    float ga = dot(viewDirection-surfaceNormal*nv, surfaceNormal-surfaceNormal*nl);

    return max(0.0, nl) * (a + b*max(0.0, ga) * sqrt((1.0-nv*nv)*(1.0-nl*nl)) / max(nl, nv));
}

vec3 blinn_phong_brdf(vec3 in_direction, vec3 out_direction, vec3 normal){

    vec3 halfwayVector = normalize(out_direction + in_direction);

    float kL = 0.9;
    float kG = 0.1;
    float s = 100.0;
    vec3 pL = albedo;
    vec3 pG = vec3(1.0);

    vec3 retVec = kL * (pL / pi) + kG *(pG*((8.0+s)/(8.0*pi))* pow(max(dot(normal, halfwayVector), 0.0), s));
    return retVec;

}

vec3 cook_torrance_brdf(vec3 in_direction, vec3 out_direction, vec3 normal){

    // The "metallic workflow"
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 halfwayVector = normalize(out_direction + in_direction);

    float NDF = DistributionBeckmann(normal, halfwayVector, roughness);
    //float NDF = DistributionGGX(normal, halfwayVector, roughness);

    float G   = GeometryOriginalCookTorrance(normal, out_direction, in_direction);
    //float G   = GeometrySmith(normal, out_direction, in_direction, roughness);

    vec3 F    = fresnelSchlickApprox(clamp(dot(halfwayVector, out_direction), 0.0, 1.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, out_direction), 0.0) * max(dot(normal, in_direction), 0.0) + 0.0001;// add some margin to avoid divide-by-zero
    vec3 specular = numerator / denominator;

    // kS is equal to the fresnel
    vec3 kS = F;

    vec3 kD = vec3(1.0) - kS;

    kD *= 1.0 - metallic;

    return kD * albedo / pi + specular;

}

void main()
{

    vec3 N = normalize(Normal);
    vec3 V = normalize(CameraPosition - WorldPos);

    vec3 LightOutTotal = vec3(0.0);
    for (int l = 0; l < LightsCount; ++l)
    {
        vec3 L = normalize(LightPositions[l] - WorldPos);

        // Incident angle
        float NdotL = max(dot(N, L), 0.0);

        // Incoming radiance from the light source
        float distance = length(LightPositions[l] - WorldPos) * 0.002;
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = LightColors[l] * attenuation;

        // LightOutTotal += radiance * lambertian_brdf(L, V, N) * NdotL;
        LightOutTotal += radiance * cook_torrance_brdf(L, V, N) * NdotL;
        // LightOutTotal += radiance * oren_nayar_brdf(L, V, N) * NdotL;

    }

    // HDR tonemapping
    LightOutTotal = LightOutTotal / (LightOutTotal + vec3(1.0));

    // Gamma correction
    LightOutTotal = pow(LightOutTotal, vec3(1.0/2.2));

    FragColor = vec4(LightOutTotal, 1.0);
}
