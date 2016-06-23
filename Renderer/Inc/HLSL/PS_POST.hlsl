SamplerState pointWrap : register(s0);
SamplerState pointClamp : register(s1);
Texture2D backbuffer : register(t9);
SamplerState anisotropicWrap : register(s6);
SamplerState anisotropicClamp : register(s7);
#include "../ConstantBuffer.h"
#include "InputLayout.hlsli"


float4 main(VEROUT_POSTEXCOORD3 Input) : SV_TARGET
{


	int iSeed = 156135;
	float fNoiseAmount = 0.010f;
	float NoiseX = iSeed  * sin(Input.texcoord.x * Input.texcoord.y + gTotalTime);
	NoiseX = fmod(NoiseX, 8) * fmod(NoiseX, 4);

	// Use our distortion factor to compute how much it will affect each
	// texture coordinate
	float DistortX = fmod(NoiseX, fNoiseAmount);
	float DistortY = fmod(NoiseX, fNoiseAmount + 0.002);

	// Create our new texture coordinate based on our distortion factor
	float2 DistortTex = float2(DistortX, DistortY);

	// Use our new texture coordinate to look-up a pixel in ColorMapSampler.
	float4 Color = backbuffer.Sample(pointClamp, Input.texcoord.xy + DistortTex);
	

	// Keep our alphachannel at 1.
	Color.a = 1.0f;
	return Color;
}

