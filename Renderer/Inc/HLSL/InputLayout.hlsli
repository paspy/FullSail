#ifndef INPUTLAYOUT_HLSLI
#define INPUTLAYOUT_HLSLI
#include "../SharedDefines.h"

//VS_In
struct _regAlign VERIN_POS
{
    float3 position SEMANTIC(POSITION0);
};

struct _regAlign VERIN_POSNORUVTAN
{
	float3 position SEMANTIC(POSITION0);
	float3 normal SEMANTIC(NORMAL0);
	float3 texcoord SEMANTIC(TEXCOORD0);
    float3 tangent SEMANTIC(TANGENT0);
};

//VS_Out
struct _regAlign VEROUT_POSTEXCOORD3
{
    float4 position SEMANTIC(SV_POSITION);
    float3 texcoord SEMANTIC(TEXCOORD0);
};

struct _regAlign VEROUT_POSNORDIFFTBN
{
	float4 position SEMANTIC(SV_POSITION);
	float3 normal  SEMANTIC(NORMAL);
    float4 posW SEMANTIC(TEXCOORD0);
    float2 texcoord SEMANTIC(TEXCOORD1);
    float3x3 tbn SEMANTIC(TEXCOORD3);
	float clip SEMANTIC(SV_ClipDistance0);
};

struct _regAlign VEROUT_POSNORDIFFREFELECT
{
    float4 position SEMANTIC(SV_POSITION);
    float3 normal SEMANTIC(NORMAL);
    float4 posW SEMANTIC(TEXCOORD0);
    float3 reflectionVector SEMANTIC(TEXCOORD1);
    float2 texcoord SEMANTIC(TEXCOORD2);
    float3x3 tbn SEMANTIC(TEXCOORD3);
};

struct _regAlign VEROUT_PosNorDiffUVTan
{
	float4 position SEMANTIC(SV_POSITION);
    float4 posW SEMANTIC(TEXCOORD0);
	float3 normal  SEMANTIC(NORMAL);
	float4 color SEMANTIC(COLOR);
	float2 texcoord SEMANTIC(TEXCOORD1);
	float4 tangent SEMANTIC(TANGENT);
};

// terrain input layouts
struct _regAlign VERIN_POSUVBOUNDY {
    float3 PositionL SEMANTIC(POSITION);
    float2 TexCoord  SEMANTIC(TEXCOORD0);
    float2 BoundsY   SEMANTIC(TEXCOORD1);
};

struct _regAlign VEROUT_POSUVBOUNDY {
    float3 PositionW SEMANTIC(POSITION);
    float2 TexCoord  SEMANTIC(TEXCOORD0);
    float2 BoundsY   SEMANTIC(TEXCOORD1);
};

struct _regAlign PatchTess {
    float EdgeTess[4]   SEMANTIC(SV_TessFactor);
    float InsideTess[2] SEMANTIC(SV_InsideTessFactor);
};

struct _regAlign HSOUT_POSUV {
    float3 PositionW   SEMANTIC(POSITION);
    float2 TexCoord    SEMANTIC(TEXCOORD0);
};

struct _regAlign DSOUT_POSHPOSWUV0UV1 {
    float4 PositionH  SEMANTIC(SV_POSITION);
    float3 PositionW  SEMANTIC(POSITION);
    float2 TexCoord   SEMANTIC(TEXCOORD0);
    float2 TiledTex	  SEMANTIC(TEXCOORD1);
};

// Particle layouts
struct _regAlign Particle_POSVELSIZAGETYP {
    float3  InitialPosW SEMANTIC( POSITION );
    float3  InitialVelW SEMANTIC( VELOCITY );
    float2  SizeW       SEMANTIC( SIZE );
    float   Age         SEMANTIC( AGE );
    uint    Type        SEMANTIC( TYPE );
};

struct _regAlign VSOUT_Particle {
    float3 PositionW  SEMANTIC( POSITION );
    float2 SizeW	  SEMANTIC( SIZE );
    float4 Color	  SEMANTIC( COLOR );
    uint   Type		  SEMANTIC( TYPE );
};

struct _regAlign GSOUT_Particle {
    float4 PositionH  SEMANTIC( SV_Position );
    float4 Color	  SEMANTIC( COLOR );
    float2 TexCoord   SEMANTIC( TEXCOORD );
};

#endif //INPUTLAYOUT_HLSLI