#include "InputLayout.hlsli"
#include "terrainSamplers.hlsli"
#include "../ConstantBuffer.h"
#include "CommonShaderFunctions.hlsli"

[domain( "quad" )]
DSOUT_POSHPOSWUV0UV1 main( PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HSOUT_POSUV, 4> quad ) {
    DSOUT_POSHPOSWUV0UV1 dsOutput = (DSOUT_POSHPOSWUV0UV1)0;

    // Bilinear interpolation.
    dsOutput.PositionW = lerp(
        lerp( quad[0].PositionW, quad[1].PositionW, uv.x ),
        lerp( quad[2].PositionW, quad[3].PositionW, uv.x ),
        uv.y
    );

    dsOutput.TexCoord = lerp(
        lerp( quad[0].TexCoord, quad[1].TexCoord, uv.x ),
        lerp( quad[2].TexCoord, quad[3].TexCoord, uv.x ),
        uv.y
    );

    float2 gTexScale = 75.0f;

    // Tile layer textures over terrain.
    dsOutput.TiledTex = dsOutput.TexCoord*gTexScale;

    // Displacement mapping
    dsOutput.PositionW.y = gHeightMap.SampleLevel( HeightmapSamplerState, dsOutput.TexCoord, 0 ).r - 10.0f;
    //dsOutput.PositionW.y = 0;
    dsOutput.PositionH = mul( float4( dsOutput.PositionW, 1.0f ), gViewProj );

    return dsOutput;
}

