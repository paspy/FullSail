#include "InputLayout.hlsli"
#include "Samplers.hlsli"
#include "../ConstantBuffer.h"


VEROUT_POSNORDIFFTBN main(VERIN_POSNORUVTAN input)
{

    VEROUT_POSNORDIFFTBN output = (VEROUT_POSNORDIFFTBN) 0;
    output.position = mul(float4(input.position, 1), gWorld);
	output.position = mul(output.position, gView);
	if (gCameraShake.x > 0.0f)
	{
		output.position.xz += cos((gTotalTime / gCameraShake.y) * 2 * 3.14159) * gCameraShake.x;
	}
	
	output.position = mul(output.position, gProj);

    output.texcoord = input.texcoord.xy;
    output.posW = mul(float4(input.position, 1), gWorld);
    
    float3 normalWS = mul(input.normal, (float3x3)gWorld).xyz;
    float3 tangentWS = mul(input.tangent, (float3x3)gWorld).xyz;

    output.normal = normalWS;
    output.tbn[0] = tangentWS;
    output.tbn[1] = cross(normalWS, tangentWS);
    output.tbn[2] = normalWS;
	output.clip = dot(output.posW, float4(0.0f,1.0f,0.0f,0.0f));
    return output;
}