#include "InputLayout.hlsli"
#include "terrainSamplers.hlsli"
#include "../ConstantBuffer.h"
#include "Lighting.hlsli"

float4 main( DSOUT_POSHPOSWUV0UV1 psInput ) : SV_Target {
    // Estimate normal and tangent using central differences.
    float2 leftTex = psInput.TexCoord + float2( -gTexelCellSpaceU, 0.0f );
    float2 rightTex = psInput.TexCoord + float2( gTexelCellSpaceU, 0.0f );
    float2 bottomTex = psInput.TexCoord + float2( 0.0f, gTexelCellSpaceV );
    float2 topTex = psInput.TexCoord + float2( 0.0f, -gTexelCellSpaceV );

    float leftY = gHeightMap.SampleLevel( HeightmapSamplerState, leftTex, 0 ).r;
    float rightY = gHeightMap.SampleLevel( HeightmapSamplerState, rightTex, 0 ).r;
    float bottomY = gHeightMap.SampleLevel( HeightmapSamplerState, bottomTex, 0 ).r;
    float topY = gHeightMap.SampleLevel( HeightmapSamplerState, topTex, 0 ).r;

    float3 tangent = normalize( float3( 2.0f*gWorldCellSpace, rightY - leftY, 0.0f ) );
    float3 biTangent = normalize( float3( 0.0f, bottomY - topY, -2.0f*gWorldCellSpace ) );
    float3 normalW = cross( tangent, biTangent );

    // Texturing

    // Sample layers in texture array.
    float4 c0 = gLayerMapArray0.Sample( LinearSamplerState, psInput.TiledTex );
    float4 c1 = gLayerMapArray1.Sample( LinearSamplerState, psInput.TiledTex );
    float4 c2 = gLayerMapArray2.Sample( LinearSamplerState, psInput.TiledTex );
    float4 c3 = gLayerMapArray3.Sample( LinearSamplerState, psInput.TiledTex );
    float4 c4 = gLayerMapArray4.Sample( LinearSamplerState, psInput.TiledTex );

    // Sample the blend map.
    float4 t = gBlendMap.Sample( LinearSamplerState, psInput.TexCoord );

    // Blend the layers on top of each other.
    float4 texColor = c0;
    texColor = lerp( texColor, c1, t.r );
    texColor = lerp( texColor, c2, t.g );
    texColor = lerp( texColor, c3, t.b );
    texColor = lerp( texColor, c4, t.a );

    // Lighting.
    LightingResult lit = DoLighting( lightList, gCameraPos, psInput.PositionW, normalW );
    return texColor * float4( lit.Diffuse.rgb, 1.0f );

}