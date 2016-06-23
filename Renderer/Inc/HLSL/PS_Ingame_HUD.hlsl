#include "InputLayout.hlsli"
#include "../ConstantBuffer.h"
Texture2D hudTexture : register(t0);
SamplerState anisotropicClamp : register(s7);


float4 main(VEROUT_POSTEXCOORD3 Input) : SV_TARGET
{
	if (0 != gHasTexture)
	{
		float4 alphaCutter = hudTexture.Sample(anisotropicClamp, Input.texcoord.xy);
		if (alphaCutter.a < 0.5f)
			discard;
        return alphaCutter * gFillColor;
    }
	else
	{
		return gFillColor;
	}
	
}