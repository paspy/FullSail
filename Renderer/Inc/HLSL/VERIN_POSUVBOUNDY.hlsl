#include "InputLayout.hlsli"

float4 main( VERIN_POSUVBOUNDY input ) : SV_POSITION
{
    return float4( input.PositionL, 1 );
}