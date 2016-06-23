
#include "InputLayout.hlsli"


VEROUT_POSTEXCOORD3 main(uint vI : SV_VERTEXID)
{
	float3 inTex = float3(vI % 2, vI % 4 / 2,0.0f);
	VEROUT_POSTEXCOORD3 Out = (VEROUT_POSTEXCOORD3)0;
	Out.position = float4((inTex.x - 0.5f) * 2, -(inTex.y - 0.5f) * 2, 0, 1);
	Out.texcoord = inTex;
	return Out;
}

