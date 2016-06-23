#pragma pack_matrix(row_major)
#include "Samplers.hlsli"
#include "../ConstantBuffer.h"

RWByteAddressBuffer vertexBuffer : register(u3);


#define WATERPLANELENGTH 100.0f
#define HEIGHT_SCALE 3.0f
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GroupID : SV_GroupID)
{
	int index = GroupID.x + GroupID.y * 101;
	float3 posL = asfloat(vertexBuffer.Load3(index * 48));
	float4 posW = mul(float4(posL, 1), gWorld);
	float2 uv = float2(posW.x / WATERPLANELENGTH, posW.z / WATERPLANELENGTH);
	posL.y = HEIGHT_SCALE * heightMap.SampleLevel(anisotropicWrap, (uv + gWaterScrollSpeed ), 1).x - 0.8f;
	vertexBuffer.Store(index * 48 + 4, asuint(posL.y));


}