#include "InputLayout.hlsli"
#include "../ConstantBuffer.h"
#include "CommonShaderFunctions.hlsli"
#include "Samplers.hlsli"
#include "Lighting.hlsli"

#define BOUNDRY		300.0f

float4 main(VEROUT_POSNORDIFFREFELECT input) : SV_TARGET
{


	float2 texSize = (float2)0;
	heightMap.GetDimensions(texSize.x, texSize.y);
	float3 normal = filterNormal(anisotropicWrap, heightMap, 2.0f, 1, input.texcoord.xy, 1.0f / texSize, texSize.x / texSize.y);
	normal = mul(normal, input.tbn);
	normal = normalize(normal);
	normal = normalize(input.normal.xyz + normal);

	float3 eyeDir = normalize(input.posW.xyz - gCameraPos);

	float3 reflectionVector = reflect(eyeDir, normal);
	float4 envReflectColor = skybox.Sample(anisotropicWrap, reflectionVector);
	envReflectColor = pow(abs(envReflectColor), 2.2);

	float3 Refract = refract(eyeDir, normal, 0.75f);
	float4 envRefractColor = skybox.Sample(anisotropicWrap, Refract);
	envRefractColor = pow(abs(envRefractColor), 2.2);



	float4 diffuse = DiffuseColor;
	if (HasDiffuseTexture)
	{
		float4 diffuseTex = diffuseMap.Sample(anisotropicWrap, input.texcoord);
		diffuseTex = pow(abs(diffuseTex), 2.2);
		if (any(diffuse.rgb))
		{
			diffuse *= diffuseTex;
		}
		else
		{
			diffuse = diffuseTex;
		}
	}


	float4 ambient = AmbientColor;
	ambient *= GlobalAmbient;

	LightingResult lit = DoLighting(lightList, gCameraPos, input.posW.xyz, normal);

	diffuse*= float4(lit.Diffuse.rgb, 1.0f);
	envReflectColor *= float4(lit.Diffuse.rgb, 1.0f);
	envRefractColor *= float4(lit.Diffuse.rgb, 1.0f);

	float4 specular = 0;
	if (SpecularPower > 1.0f) // If specular power is too low, don't use it.
	{
		specular = SpecularColor;

		specular *= lit.Specular;
	}

    if (input.posW.z >BOUNDRY || input.posW.z <- BOUNDRY ||
		input.posW.x >BOUNDRY || input.posW.x < - BOUNDRY )
	{
		envReflectColor = lerp(envReflectColor, float4(envReflectColor.x, envReflectColor.x, envReflectColor.x, envReflectColor.w), 0.9f);
	}
	return float4((ambient + diffuse+ envReflectColor + specular+ envRefractColor).rgb,
					 0.99f);




}
