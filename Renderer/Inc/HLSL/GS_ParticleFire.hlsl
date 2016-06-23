#include "InputLayout.hlsli"
#include "../ConstantBuffer.h"

// The draw GS just expands points into camera facing quads.
[maxvertexcount( 4 )]
void main( point VSOUT_Particle gsInput[1], inout TriangleStream<GSOUT_Particle> triStream ) {
    float2 gQuadTexC[4] = {
        float2( 0.0f, 1.0f ),
        float2( 0.0f, 0.0f ),
        float2( 1.0f, 1.0f ),
        float2( 1.0f, 0.0f )
    };
    // do not draw emitter particles.
    if ( gsInput[0].Type != PT_EMITTER ) {

        // Compute world matrix so that billboard faces the camera.
        float3 look = normalize( gCameraPos - gsInput[0].PositionW );
        float3 right = normalize( cross( float3( 0, 1, 0 ), look ) );
        float3 up = cross( look, right );

        // Compute triangle strip vertices (quad) in world space.
        float halfWidth = 0.5f*gsInput[0].SizeW.x;
        float halfHeight = 0.5f*gsInput[0].SizeW.y;

        //float4 v[4];
        //v[0] = float4( gsInput[0].PositionW + halfWidth*right - halfHeight*up, 1.0f );
        //v[1] = float4( gsInput[0].PositionW + halfWidth*right + halfHeight*up, 1.0f );
        //v[2] = float4( gsInput[0].PositionW - halfWidth*right - halfHeight*up, 1.0f );
        //v[3] = float4( gsInput[0].PositionW - halfWidth*right + halfHeight*up, 1.0f );

        //// Transform quad vertices to world space and output them as a triangle strip.
        //GSOUT_Particle gout;
        //[unroll]
        //for ( int i = 0; i < 4; ++i ) {
        //    gout.PositionH = mul( v[i], gViewProj );
        //    gout.TexCoord = gQuadTexC[i];
        //    gout.Color = gsInput[0].Color;
        //    triStream.Append( gout );
        //}


        float4 v[4];
        v[0] = mul( float4( gsInput[0].PositionW, 1.0f ), gView );
        v[0].x += halfWidth;
        v[0].y -= halfHeight;

        v[2] = mul( float4( gsInput[0].PositionW, 1.0f ), gView );
        v[2].x += halfWidth;
        v[2].y += halfHeight;

        v[1] = mul( float4( gsInput[0].PositionW, 1.0f ), gView );
        v[1].x -= halfWidth;
        v[1].y -= halfHeight;

        v[3] = mul( float4( gsInput[0].PositionW, 1.0f ), gView );
        v[3].x -= halfWidth;
        v[3].y += halfHeight;

        // Transform quad vertices to world space and output them as a triangle strip.
        GSOUT_Particle gout;
        [unroll]
        for ( int i = 0; i < 4; ++i ) {
            gout.PositionH = mul( v[i], gProj );
            gout.TexCoord = gQuadTexC[i];
            gout.Color = gsInput[0].Color;
            triStream.Append( gout );
        }

    }
}
