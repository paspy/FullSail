SamplerState pointWrap : register(s0);
SamplerState pointClamp : register(s1);
Texture2D backbuffer : register(t9);
SamplerState anisotropicWrap : register(s6);
SamplerState anisotropicClamp : register(s7);
#include "../ConstantBuffer.h"
#include "InputLayout.hlsli"


float4 main(VEROUT_POSTEXCOORD3 Input) : SV_TARGET
{

    float4 color = backbuffer.Sample(anisotropicClamp, Input.texcoord.xy);
    float2 resolution = (float2)0;
    backbuffer.GetDimensions(resolution.x, resolution.y);
    float2 position = (Input.position.xy / resolution.y) - float2(0.5 * resolution.x / resolution.y, 0.5);
    float len = length(position);
    float RADIUS = 0.7f;
    float SOFTNESS = 0.3f;
    float vignette = smoothstep(RADIUS, RADIUS - SOFTNESS, len);

    color.rgb *= vignette;
    return color;
}