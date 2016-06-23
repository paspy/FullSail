#include "InputLayout.hlsli"
#include "CommonShaderFunctions.hlsli"
#include "Samplers.hlsli"
#include "../ConstantBuffer.h"

#define WATERPLANELENGTH 100.0f




VEROUT_POSNORDIFFREFELECT main(VERIN_POSNORUVTAN input)
{

	VEROUT_POSNORDIFFREFELECT output = (VEROUT_POSNORDIFFREFELECT)0;

	output.posW = mul(float4(input.position, 1), gWorld);
	output.texcoord.xy = output.posW.xz / 50.0f;
	output.texcoord.xy += gWaterScrollSpeed;


	output.position = mul(float4(input.position, 1), gWorld);
	output.position = mul(output.position, gView);
	if (gCameraShake.x > 0.0f)
	{
		output.position.xz += cos((gTotalTime / gCameraShake.y) * 2 * 3.14159) * gCameraShake.x;
	}


	output.position = mul(output.position, gProj);



	float2 texSize;
	heightMap.GetDimensions(texSize.x, texSize.y);
	float3 normal = filterNormal(anisotropicWrap, heightMap, 4.0f, 1, output.texcoord.xy, 1.0f / texSize, texSize.x / texSize.y);



	float3 normalWS = mul(normal, (float3x3)gWorld).xyz;

	float3 tangentWS;

	float3 c1 = cross(normal, float3(0.0, 0.0, 1.0));
	float3 c2 = cross(normal, float3(0.0, 1.0, 0.0));

	if (length(c1) > length(c2))
	{
		tangentWS = c1;
	}
	else
	{
		tangentWS = c2;
	}
	tangentWS = mul(tangentWS, (float3x3)gWorld).xyz;
	tangentWS = normalize(tangentWS);
	output.normal = normalWS;
	output.tbn[0] = tangentWS;
	output.tbn[1] = cross(normalWS, tangentWS);
	output.tbn[2] = normalWS;
	return output;
}