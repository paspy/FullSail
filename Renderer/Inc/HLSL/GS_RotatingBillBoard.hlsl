#include "InputLayout.hlsli"
#include "../ConstantBuffer.h"

// The draw GS just expands points into camera facing quads.
[maxvertexcount(4)]
void main(point VSOUT_Particle gsInput[1], inout TriangleStream<GSOUT_Particle> triStream) {

	float2 gQuadTexC[4] = {
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};

	// do not draw emitter particles.
	if (gsInput[0].Type != PT_EMITTER) {

		// Compute triangle strip vertices (quad) in view space.
		float halfWidth = 0.5f*gsInput[0].SizeW.x;
		float halfHeight = 0.5f*gsInput[0].SizeW.y;

		float4 v[4];

		v[3] = v[1] = v[2] = v[0] = float4(gsInput[0].PositionW, 1.0f);

		//Applying the View
		v[0] = mul(v[0], gView);
		v[1] = mul(v[1], gView);
		v[2] = mul(v[2], gView);
		v[3] = mul(v[3], gView);

		//Applying the roation
		v[0] = mul(v[0], gRotationMat);
		v[1] = mul(v[1], gRotationMat);
		v[2] = mul(v[2], gRotationMat);
		v[3] = mul(v[3], gRotationMat);

		//Translating
		v[0].x += halfWidth;
		v[0].y -= halfHeight;


		v[2].x += halfWidth;
		v[2].y += halfHeight;

		v[1].x -= halfWidth;
		v[1].y -= halfHeight;


		v[3].x -= halfWidth;
		v[3].y += halfHeight;
	

		
		// Transform quad vertices to projection space and output them as a triangle strip.
		GSOUT_Particle gout;
		[unroll]
		for (int i = 0; i < 4; i++) {
			gout.PositionH = mul(v[i], gProj);
			gout.TexCoord = gQuadTexC[i];
			gout.Color = gsInput[0].Color;
			triStream.Append(gout);
		}
	}
}