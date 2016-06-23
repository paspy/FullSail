#include "InputLayout.hlsli"
#include "../ConstantBuffer.h"

VSOUT_Particle main( Particle_POSVELSIZAGETYP vsInput ) {
    VSOUT_Particle vout;

    float t = vsInput.Age;
    // constant acceleration equation
    vout.PositionW = 0.5f*t*t/**gEmitDirW.xyz*/ + t*vsInput.InitialVelW + vsInput.InitialPosW;
    //vout.PositionW = 0.5f*t*t* ( float4( gEmitDirW.xyz, 0.0f ) ).xyz + t* ( float4( vsInput.InitialVelW, 0.0f ) ).xyz + mul( float4( vsInput.InitialPosW, 1.0f ), gParticleWorld ).xyz;

    // fade color with time
    float speedLerp = smoothstep( -1.0f, 10.0f, length(gEmitVelocity) );

    float opacity = 1.0f - smoothstep( speedLerp, 1.0f, t* ( /*1.0f /*/ gParticleLife ) );

    vout.Color = float4( gEmitColor.rgb, opacity );

    vout.SizeW = vsInput.SizeW * t*2.0f + vsInput.SizeW;
    vout.Type = vsInput.Type;

    return vout;
}