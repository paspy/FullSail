#ifndef LIGHTING_HLSLI
#define LIGHTING_HLSLI


#include "../Light.h"
#include "../ConstantBuffer.h"

#define NUM_LIGHTS 3
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2



StructuredBuffer<Light> lightList : register(t10);
struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};
 

float4 Diffuse(Light light, float3 L, float3 N)
{
    float NdotL = saturate(dot(N, L));
    return light.Color * NdotL;
}

float4 Specular(Light light, float3 V, float3 L, float3 N)
{

	float3 HalfWay = normalize(V + -L);
	float NDotH = saturate(dot(HalfWay, N));
	pow(NDotH, SpecularPower) * SpecularScale;
    return light.Color * pow(NDotH, SpecularPower) * SpecularScale;
}

float Attenuation(Light light, float d)
{
    return 1.0f - smoothstep(0, light.Range, d);
}


LightingResult DoDirectionalLight(Light light, float3 V, float3 N)
{
    LightingResult result;
 
    float3 L = -normalize(light.DirectionWS.xyz);
 
    result.Diffuse = Diffuse(light, L, N) * light.Intensity;
    result.Specular = Specular(light, V, L, -N) * light.Intensity;
 
    return result;
}


LightingResult DoPointLight(Light light ,float3 V, float3 P, float3 N)
{
	LightingResult result;

	float3 L = -(light.PositionWS.xyz - P);
	float distance = length(L);
	L /= distance;

	float attenuation =  Attenuation(light, distance);

	result.Diffuse = Diffuse(light, L, N) *
		attenuation * light.Intensity;
	result.Specular = Specular(light, V, L, N) *
		attenuation * light.Intensity;

	return result;
}

float DoSpotCone(Light light, float3 L)
{
	// If the cosine angle of the light's direction 
	// vector and the vector from the light source to the point being 
	// shaded is less than minCos, then the spotlight contribution will be 0.
	float minCos = cos(radians(light.SpotlightAngle));
	// If the cosine angle of the light's direction vector
	// and the vector from the light source to the point being shaded
	// is greater than maxCos, then the spotlight contribution will be 1.
	float maxCos = lerp(minCos, 1, 0.5f);
	float cosAngle = dot(light.DirectionWS.xyz, -L);
	// Blend between the minimum and maximum cosine angles.
	return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult DoSpotLight(Light light, float3 V, float3 P, float3 N)
{
	LightingResult result;

	float3 L = -(light.PositionWS.xyz - P);
	float distance = length(L);
	L = L / distance;

	float attenuation = Attenuation(light, distance);
	float spotIntensity = DoSpotCone(light, L);

	result.Diffuse = Diffuse(light, L, N) *
		attenuation * spotIntensity * light.Intensity;
	result.Specular = Specular(light, V, L, N) *
		attenuation * spotIntensity * light.Intensity;

	return result;
}

LightingResult DoLighting(StructuredBuffer<Light> lights, float3 eyePos, float3 P, float3 N)
{
    float3 V = normalize(eyePos - P);
 
    LightingResult totalResult = (LightingResult) 0;
 
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        LightingResult result = (LightingResult) 0;
        if (lights[i].Enabled == 0)
            continue;
        // Skip point and spot lights that are out of range of the point being shaded.
        if (lights[i].Type != 0 &&
             length(lights[i].PositionWS.xyz - P) > lights[i].Range)
            continue;
 
        switch (lights[i].Type)
        {
            case 0:
                {
                    result = DoDirectionalLight(lights[i], V, N);
                }
                break;
            case 1:
                {
                   result = DoPointLight(lights[i], V, P, N);
                }
                break;
            case 2:
                {
                   result = DoSpotLight(lights[i], V, P, N);
                }
                break;
        }
        totalResult.Diffuse += pow(abs(result.Diffuse), 1 / 2.2);
        totalResult.Specular += pow(abs(result.Specular), 1 / 2.2);
    }
	//totalResult.Diffuse = pow(abs(totalResult.Diffuse), 1 / 2.2);
	//totalResult.Specular = pow(abs(totalResult.Specular), 1 / 2.2);

    return  totalResult;
}

#endif //LIGHTING_HLSLI