#include "InputLayout.hlsli"
#include "../ConstantBuffer.h"

VEROUT_POSTEXCOORD3 main(VERIN_POS input)
{
    VEROUT_POSTEXCOORD3 output = (VEROUT_POSTEXCOORD3) 0;
    output.position = mul(float4(input.position, 1), gMVP);
    output.texcoord = input.position.xyz;
    return output;
}