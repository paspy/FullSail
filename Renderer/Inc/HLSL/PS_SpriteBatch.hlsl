Texture2D<float4> Texture : register( t0 );
sampler TextureSampler : register( s0 );

cbuffer CusSpriteCB : register( b1 ) {
    float threshold;
    float isAnim;
    float customFloat2;
    float customFloat3;
};

float4 main( float4 color: COLOR0, float2 texCoord: TEXCOORD0 ) : SV_TARGET0
{
	float4 rtn = Texture.Sample(TextureSampler, texCoord);
	if (isAnim != 0)
	{
		float blendRatio = 0.01f;
		if (rtn.r < threshold - blendRatio)
		{
			discard;
			return float4(0,0,0,0);
		}
		else
			return float4(color.rgb * rtn.a, rtn.a);
	}
	else
		return rtn * color;
}
