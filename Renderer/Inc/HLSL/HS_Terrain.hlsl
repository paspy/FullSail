#include "InputLayout.hlsli"
#include "terrainSamplers.hlsli"
#include "../ConstantBuffer.h"
#include "CommonShaderFunctions.hlsli"

float CalcTessFactor( float3 p ) {
    //float d = distance( p, gCameraPosW.xyz );
    float d = distance( p, gCameraPos.xyz );
    float s = saturate( ( d - gMinDist ) / ( gMaxDist - gMinDist ) );
    return pow( 2, ( lerp( gMaxTess, gMinTess, s ) ) );
}

PatchTess ConstantHS( InputPatch<VEROUT_POSUVBOUNDY, 4> patch, uint patchID : SV_PrimitiveID ) {
    PatchTess pt;

    float minY = patch[0].BoundsY.x;
    float maxY = patch[0].BoundsY.y;

    float3 vMin = float3( patch[2].PositionW.x, minY, patch[2].PositionW.z );
    float3 vMax = float3( patch[1].PositionW.x, maxY, patch[1].PositionW.z );

    float3 boxCenter = 0.5f*( vMin + vMax );
    float3 boxExtents = 0.5f*( vMax - vMin );

    // Frustum culling
    if ( AabbOutsideFrustumTest( boxCenter, boxExtents, gWorldFrustumPlanes ) ) {
        pt.EdgeTess[0] = 0.0f;
        pt.EdgeTess[1] = 0.0f;
        pt.EdgeTess[2] = 0.0f;
        pt.EdgeTess[3] = 0.0f;

        pt.InsideTess[0] = 0.0f;
        pt.InsideTess[1] = 0.0f;

        return pt;

    } else {
        float3 e0 = 0.5f*( patch[0].PositionW + patch[2].PositionW );
        float3 e1 = 0.5f*( patch[0].PositionW + patch[1].PositionW );
        float3 e2 = 0.5f*( patch[1].PositionW + patch[3].PositionW );
        float3 e3 = 0.5f*( patch[2].PositionW + patch[3].PositionW );
        float3  c = 0.25f*( patch[0].PositionW + patch[1].PositionW + patch[2].PositionW + patch[3].PositionW );

        pt.EdgeTess[0] = CalcTessFactor( e0 );
        pt.EdgeTess[1] = CalcTessFactor( e1 );
        pt.EdgeTess[2] = CalcTessFactor( e2 );
        pt.EdgeTess[3] = CalcTessFactor( e3 );

        pt.InsideTess[0] = CalcTessFactor( c );
        pt.InsideTess[1] = pt.InsideTess[0];

        return pt;
    }
}

[domain( "quad" )]
[partitioning( "fractional_even" )]
[outputtopology( "triangle_cw" )]
[outputcontrolpoints( 4 )]
[patchconstantfunc( "ConstantHS" )]
[maxtessfactor( 64.0f )]
HSOUT_POSUV main( InputPatch<VEROUT_POSUVBOUNDY, 4> p, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID ) {
    HSOUT_POSUV hsOutput = (HSOUT_POSUV)0;

    // Pass through shader.
    hsOutput.PositionW = p[i].PositionW;
    hsOutput.TexCoord = p[i].TexCoord;

    return hsOutput;
}

