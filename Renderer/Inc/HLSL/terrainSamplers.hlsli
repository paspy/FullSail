#ifndef TerrainSAMPLERS_HLSLI
#define TerrainSAMPLERS_HLSLI

Texture2D gBlendMap         : register( t0 );
Texture2D gHeightMap        : register( t1 );
Texture2D gLayerMapArray0   : register( t2 );
Texture2D gLayerMapArray1   : register( t3 );
Texture2D gLayerMapArray2   : register( t4 );
Texture2D gLayerMapArray3   : register( t5 );
Texture2D gLayerMapArray4   : register( t6 );

SamplerState LinearSamplerState     : register( s0 );
SamplerState HeightmapSamplerState  : register( s1 );

#endif //SAMPLERS_HLSLI