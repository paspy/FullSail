#ifndef CONSTANT_BUFFERS_H
#define CONSTANT_BUFFERS_H
#include "SharedDefines.h"

CONSTANT_BUFFER_BEGIN(cbPerObject, b0)
float4x4	gMVP;
float4x4	gWorld;
int			gBeingHit;
float3		gDarkWaterSize;
float4		gDarkWaterCenter;
#ifdef __cplusplus
const static int REGISTER_SLOT = 0;
cbPerObject() { SecureZeroMemory( this, sizeof( *this ) ); }
#endif // __cplusplus
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN(cbPerCamera, b1)
float4x4	gView;
float4x4    gProj;
float4x4	gViewProj;
float4x4	gInvViewProj;

float3		gCameraPos;
float		gDeltaTime;

float3		gCameraDir;
float       gTotalTime;

float4      gWorldFrustumPlanes[6]; // 96

float       gWaterScrollSpeed;
float3		gCameraShake; //x for amount y for duration

#ifdef __cplusplus
const static int REGISTER_SLOT = 1;
cbPerCamera() { SecureZeroMemory( this, sizeof( *this ) ); }
#endif // __cplusplus
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN( cbPerMaterial, b2 )
float4  GlobalAmbient;
//-------------------------- ( 16 bytes )
float4  AmbientColor;
//-------------------------- ( 16 bytes )
float4  EmissiveColor;
//-------------------------- ( 16 bytes )
float4  DiffuseColor;
//-------------------------- ( 16 bytes )
float4  SpecularColor;
//-------------------------- ( 16 bytes )
// Reflective value.
float4  Reflectance;
//-------------------------- ( 16 bytes )
float   Opacity;
float   SpecularPower;
// For transparent materials, IOR > 0.
float   IndexOfRefraction;
int		HasAmbientTexture;
//-------------------------- ( 16 bytes )
int     HasEmissiveTexture;
int     HasDiffuseTexture;
int     HasSpecularTexture;
int     HasAOTexture;
//-------------------------- ( 16 bytes )
int     HasNormalTexture;
int     HasHeightTexture;
int     HasOpacityTexture;
float   BumpIntensity;
//-------------------------- ( 16 bytes )
float   SpecularScale;
float   AlphaThreshold;
float2  Padding;
//--------------------------- ( 16 bytes )

#ifdef __cplusplus
const static int REGISTER_SLOT = 2;
cbPerMaterial() { SecureZeroMemory( this, sizeof( *this ) ); }
#endif // __cplusplus
CONSTANT_BUFFER_END

#if 0
CONSTANT_BUFFER_BEGIN( cbPerTerrain, b3 )
// 16
float gMinDist; // When distance is minimum, the tessellation is maximum.
float gMaxDist; // When distance is maximum, the tessellation is minimum.
float gMinTess;	// Exponents for power of 2 tessellation.  The tessellation
float gMaxTess;	// range is [2^(gMinTess), 2^(gMaxTess)].  Since the maximum
                // tessellation is 64, this means gMaxTess can be at most 6
                // since 2^6 = 64.

// 16
float gTexelCellSpaceU;
float gTexelCellSpaceV;
float gWorldCellSpace;
float Pad1;

// 96
float4 gWorldFrustumPlanes[6];
#ifdef __cplusplus
const static int REGISTER_SLOT = 3;
#endif // __cplusplus
CONSTANT_BUFFER_END
#endif

CONSTANT_BUFFER_BEGIN( cbPost, b4 )
float gFadeAmount;
float padding[3];

#ifdef __cplusplus
const static int REGISTER_SLOT = 4;
cbPost() { SecureZeroMemory( this, sizeof( *this ) ); }
#endif // __cplusplus
CONSTANT_BUFFER_END

CONSTANT_BUFFER_BEGIN( cbPerParticle, b5 )
float4x4    gParticleWorld;
float4x4    gRotationMat;
float4      gEmitPosW;
float4      gEmitDirW;
float3      gEmitVelocity;
float       gParticleLife;

float2      gParticleSize;
float       gTexArrIndex;
float       gEmitSpeed;

float4      gEmitColor;

#ifdef __cplusplus
const static int REGISTER_SLOT = 5;
cbPerParticle() { SecureZeroMemory( this, sizeof( *this ) ); }
#endif // __cplusplus
CONSTANT_BUFFER_END


CONSTANT_BUFFER_BEGIN(cbPerHUD, b0)
float4		gPosW;
float4		gWH;
float4		gFillColor;
int			gHasTexture;
#ifdef __cplusplus
const static int REGISTER_SLOT = 6;
cbPerHUD() { SecureZeroMemory(this, sizeof(*this)); }
#endif // __cplusplus
CONSTANT_BUFFER_END

#endif