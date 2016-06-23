#include "InputLayout.hlsli"
#include "Samplers.hlsli"
#include "CommonShaderFunctions.hlsli"
#include "../ConstantBuffer.h"

[maxvertexcount( 2 )]
void main( point Particle_POSVELSIZAGETYP gsInput[1], inout PointStream<Particle_POSVELSIZAGETYP> ptStream ) {

    gsInput[0].Age += gDeltaTime * 0.75f;

    if ( gsInput[0].Type == PT_EMITTER ) {
        // time to emit a new particle?
        if ( gsInput[0].Age > 0.005f ) {
            float3 vRandom = GetRandVec3( 0.0f, gTotalTime, gRandomTex, trilinearWrap );
            vRandom.y = abs( vRandom.y )*0.5f;
            vRandom.x *= 0.5f;
            vRandom.z *= 0.5f;

            Particle_POSVELSIZAGETYP p;
            p.InitialPosW = gEmitPosW.xyz;
            p.InitialVelW = vRandom * 2.5f;
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