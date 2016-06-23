#include "InputLayout.hlsli"
#include "Samplers.hlsli"
#include "CommonShaderFunctions.hlsli"
#include "../ConstantBuffer.h"

[maxvertexcount( 2 )]
void main( point Particle_POSVELSIZAGETYP gsInput[1], inout PointStream<Particle_POSVELSIZAGETYP> ptStream ) {

    gsInput[0].Age += gDeltaTime;

    if ( gsInput[0].Type == PT_EMITTER ) {

        // time to emit a new particle?
        if ( gsInput[0].Age > (1.0f - smoothstep( -1.0f, 10.0f, gEmitSpeed )) * 0.1f ) {
            float3 vRandom = GetRandVec3( 0.0f, gTotalTime, gRandomTex, trilinearWrap );
            vRandom.y *= 0.0f;// abs( vRandom.y );
            vRandom.x *= 0.25f;
            vRandom.z *= 0.25f;

            Particle_POSVELSIZAGETYP p;
            //p.InitialPosW = gEmitPosW.xyz;
            p.InitialPosW = mul( gEmitPosW, gParticleWorld ).xyz;
            p.InitialVelW = gEmitVelocity + gEmitVelocity*vRandom * 0.25f;
            p.SizeW = gParticleSize;
            p.Age = 0.0f;
            p.Type = PT_FLARE;

            ptStream.Append( p );

            // reset the time to emit
            gsInput[0].Age = 0.0f;
        }

        // always keep emitters
        ptStream.Append( gsInput[0] );


    } else {
        // Specify conditions to keep particle; this may vary from system to system.
        if ( gsInput[0].Age <= gParticleLife )
            ptStream.Append( gsInput[0] );
    }
}