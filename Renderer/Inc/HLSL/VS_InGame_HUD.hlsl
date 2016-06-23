#include "InputLayout.hlsli"
#include "../ConstantBuffer.h"

VEROUT_POSTEXCOORD3 main(uint vI : SV_VERTEXID)
{
	VEROUT_POSTEXCOORD3 Out = (VEROUT_POSTEXCOORD3)0;
	Out.position = mul(gPosW, gView);
	switch (vI)
	{
	case 0:
		Out.texcoord.xy = 0.0f;
		break;
	case 1:
		Out.position.x += gWH.x * gWH.z;
		Out.texcoord.x = 1.0f;
		break;
	case 2:
		Out.position.y -= gWH.y;
		Out.texcoord.y = 1.0f;
		break;
	case 3:
		Out.position.x += gWH.x* gWH.z;
		Out.position.y -= gWH.y;
		Out.texcoord.xy = 1.0f;
		break;
	default:
		break;
	}

	Out.position = mul(Out.position, gProj);
	return Out;
}