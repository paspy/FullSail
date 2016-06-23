#include "InputLayout.hlsli"
#include "../ConstantBuffer.h"
#include "CommonShaderFunctions.hlsli"
#include "Lighting.hlsli"
#include "Samplers.hlsli"

#define BOUNDRY		300.0f
float2  Distort(float2 p)
{
	float theta = atan2(p.y, p.x);
	float radius = length(p);
	radius = pow(radius, 2);
	p.x = radius * cos(theta);
	p.y = radius * sin(theta);
	return 0.5 * (p + 1.0);
}

float4 main(VEROUT_POSNORDIFFTBN input) : SV_TARGET
{
	

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

    float alpha = diffuse.a;

    if (HasOpacityTexture)
    {
        alpha = opcityMap.Sample(anisotropicWrap, input.texcoord).r;
    }

    if (alpha < AlphaThreshold)
    {
        discard;
    }

    float4 ambient = AmbientColor;
    if (HasAmbientTexture)
    {
        float4 ambientTex = ambientMap.Sample(anisotropicWrap, input.texcoord);
        if (any(ambient.rgb))
        {
            ambient *= ambientTex;
        }
        else
        {
            ambient = ambientTex;
        }
    }
    ambient *= GlobalAmbient;
 
    float4 emissive = EmissiveColor;
    if (HasEmissiveTexture)
    {
        float4 emissiveTex = emissiveMap.Sample(anisotropicWrap, input.texcoord);
        if (any(emissive.rgb))
        {
            emissive *= emissiveTex;
        }
        else
        {
            emissive = emissiveTex;
        }
    }


    float4 normal  = (float4)0;
    if (HasNormalTexture)
    {
        normal = NormalMapping(input.tbn, normalMap, anisotropicWrap, input.texcoord);
    }
    else
    {
        normal = normalize(float4(input.normal, 0));
    }

	
    LightingResult lit = DoLighting(lightList, gCameraPos,input.posW.xyz, normal.xyz);
 
    diffuse *= float4(lit.Diffuse.rgb, 1.0f) + diffuse * ambient;
 
    float4 specular = 0;
    if (SpecularPower > 1.0f) // If specular power is too low, don't use it.
    {
        specular = SpecularColor;
        if (HasSpecularTexture)
        {
            float4 specularTex = specularMap.Sample(anisotropicWrap, input.texcoord);
            if (any(specular.rgb))
            {
                specular *= specularTex;
            }
            else
            {
                specular = specularTex;
            }
        }
        specular *= lit.Specular;
    }
	if (gBeingHit)
	{
		diffuse = diffuseMap.Sample(anisotropicWrap, Distort(input.texcoord));
	}
	if (input.posW.z > BOUNDRY || input.posW.z < -BOUNDRY
		|| input.posW.x > BOUNDRY || input.posW.x < -BOUNDRY)
	{
		diffuse = lerp(diffuse, float4(diffuse.x, diffuse.x, diffuse.x, diffuse.w), 0.9f);
	}
	float4 finalColor = float4((emissive + diffuse + specular).rgb, alpha * Opacity);
	
	return finalColor;
}
