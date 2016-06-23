#ifndef COMMONSHADERFUNCTIONS_HLSLI
#define COMMONSHADERFUNCTIONS_HLSLI

//
//float3 HemisphericAmbient(float3 ambientDown, float3 ambientRange, float3 normal, float4 lightColor)
//{
//	normal = normalize(normal);
//	lightColor = float4(lightColor.rgb * lightColor.rgb, lightColor.a);
//	float up = normal.y * 0.5 + 0.5;
//	float3 Ambient = ambientDown + up * (ambientRange + ambientDown);
//	return Ambient * lightColor.rgb;
//}

float3 filterNormal(SamplerState s, Texture2D map,float scale, uint mipLevel, float2 uv, float2 texelSize, float texelAspect)
{
	float4 h;
	h.x = scale * map.SampleLevel(s, uv + texelSize * float2(0, -1), mipLevel).r * texelAspect;
	h.y = scale * map.SampleLevel(s, uv + texelSize * float2(-1, 0), mipLevel).r * texelAspect;
	h.z = scale * map.SampleLevel(s, uv + texelSize * float2(1, 0), mipLevel).r * texelAspect;
	h.w = scale * map.SampleLevel(s, uv + texelSize * float2(0, 1), mipLevel).r * texelAspect;

	float3 n;
	n.z = h.x - h.w;
	n.x = h.y - h.z;
	n.y = 2.0f;

	return normalize(n);
}

float3 ExpandNormal(float3 n)
{
    return n * 2.0f - 1.0f;
}
 
float4 NormalMapping(float3x3 TBN, Texture2D tex, sampler s, float2 uv)
{
    float3 normal = tex.Sample(s, uv).xyz;
    normal = ExpandNormal(normal);
    normal = mul(normal, TBN);
    return normalize(float4(normal, 0));
}

bool AabbBehindPlaneTest( float3 center, float3 extents, float4 plane ) {
    float3 n = abs( plane.xyz );
    float r = dot( extents, n );
    float s = dot( float4( center, 1.0f ), plane );
    return ( ( s + r ) < 0.0f );
}

bool AabbOutsideFrustumTest( float3 center, float3 extents, float4 frustumPlanes[6] ) {
    for ( int i = 0; i < 6; ++i ) {
        if ( AabbBehindPlaneTest( center, extents, frustumPlanes[i] ) ) {
            return true;
        }
    }

    return false;
}

float3 GetRandVec3( float offset, float totalTime, Texture1D randomTex, SamplerState sampleState ) {
    float u = ( totalTime + offset );
    
    // coordinates in [-1,1]
    float3 v = randomTex.SampleLevel( sampleState, u, 0 ).xyz;

    return normalize(v);
}

#endif //COMMONSHADERFUNCTIONS_HLSLI