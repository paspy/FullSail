SamplerState pointWrap : register(s0);
SamplerState pointClamp : register(s1);
Texture2D backbuffer : register(t9);
SamplerState anisotropicWrap : register(s6);
SamplerState anisotropicClamp : register(s7);
#include "../ConstantBuffer.h"
#include "InputLayout.hlsli"


float4 main(VEROUT_POSTEXCOORD3 Input) : SV_TARGET
{

    float4 color =  backbuffer.Sample(anisotropicClamp, Input.texcoord.xy);
	color.a = 1.0f;
	float4 outputColor = color;
	outputColor.r = (color.r * 0.393) + (color.g * 0.769) + (color.b * 0.189);
	outputColor.g = (color.r * 0.349) + (color.g * 0.686) + (color.b * 0.168);
	outputColor.b = (color.r * 0.272) + (color.g * 0.534) + (color.b * 0.131);
	return outputColor;
}