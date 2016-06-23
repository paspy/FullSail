SamplerState pointWrap : register(s0);
SamplerState pointClamp : register(s1);
Texture2D backbuffer : register(t9);
SamplerState anisotropicWrap : register(s6);
SamplerState anisotropicClamp : register(s7);

#include "../ConstantBuffer.h"
#include "InputLayout.hlsli"


float4 main(VEROUT_POSTEXCOORD3 Input) : SV_TARGET
{
	float4 color = backbuffer.Sample(anisotropicWrap,Input.texcoord.xy);
	color = lerp( float4(0.0f, 0.0f, 0.0f, 0.0f),color, gFadeAmount);
	//color *= gFadeAmount;
	//color = pow(abs(color), 2.2);
	return color;
}

