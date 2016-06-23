#include "InputLayout.hlsli"
#include "../Light.h"
#include "../ConstantBuffer.h"
#include "CommonShaderFunctions.hlsli"
#include "Samplers.hlsli"

float4 main(VEROUT_POSTEXCOORD3 input) : SV_TARGET
{
	float4 environment = skybox.Sample(anisotropicWrap, input.texcoord);
	environment = pow(abs(environment), 2.2);
	return environment;
}