#include "InputLayout.hlsli"
#include "../ConstantBuffer.h"

VSOUT_Particle main( Particle_POSVELSIZAGETYP vsInput ) {
    VSOUT_Particle vout;

    float t = vsInput.Age;
    // constant acceleration equation
    //vout.PositionW = 0.5f*t*t*gEmitDirW.xyz + t*vsInput.InitialVelW + vsInput.InitialPosW;
    vout.PositionW = 0.5f*t*t* mul( float4(gEmitDirW.xyz, 0.0f ), gParticleWorld ).xyz + t* mul(float4(vsInput.InitialVelW, 0.0f), gParticleWorld ).xyz + mul( float4( vsInput.InitialPosW, 1.0f), gParticleWorld ).xyz;
    //vout.PositionW = 0.5f*t*t*gEmitDirW.xyz + t* vsInput.InitialVelW + mul( float4( vsInput.InitialPosW, 1.0f ), gParticleWorld ).xyz;
    // fade color with time
    float opacity = 1.0f - smoothstep( 0.0f, 1.0f, t / 1.0f );

    vout.Color = float4( gEmitColor.rgb, opacity );

    vout.SizeW = vsInput.SizeW;
    vout.Type = vsInput.Type;

    return vout;
}