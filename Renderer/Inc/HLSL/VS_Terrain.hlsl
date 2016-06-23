#include "InputLayout.hlsli"
#include "terrainSamplers.hlsli"
#include "../ConstantBuffer.h"


VEROUT_POSUVBOUNDY main( VERIN_POSUVBOUNDY vsInput ) {

    VEROUT_POSUVBOUNDY vsOutput = (VEROUT_POSUVBOUNDY)0;

    vsOutput.PositionW = vsInput.PositionL;
    vsOutput.PositionW.y = gHeightMap.SampleLevel( HeightmapSamplerState, vsInput.TexCoord, 0 ).r;
    vsOutput.TexCoord = vsInput.TexCoord;
    vsOutput.BoundsY = vsInput.BoundsY;

    return vsOutput;
}