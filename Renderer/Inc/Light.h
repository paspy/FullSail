#ifndef LIGHT_H
#define LIGHT_H
#include "SharedDefines.h"


struct _regAlign Light
{
	/**
	* Position for point and spot lights (World space).
	*/
	float4   PositionWS;
	//--------------------------------------------------------------( 16 bytes )
	/**
	* Direction for spot and directional lights (World space).
	*/
	float4   DirectionWS;
	//--------------------------------------------------------------( 16 bytes )
	/**
	* Color of the light. Diffuse and specular colors are not seperated.
	*/
	float4   Color;
	//--------------------------------------------------------------( 16 bytes )
	/**
	* The half angle of the spotlight cone.
	*/
	float    SpotlightAngle;
	/**
	* The range of the light.
	*/
	float    Range;

	/**
	* The intensity of the light.
	*/
	float    Intensity;

	/**
	* Disable or enable the light.
	*/
	int    Enabled;
	//--------------------------------------------------------------( 16 bytes )

	/**
	* The type of the light.
	*/
	uint    Type;
	float3  Padding;
	//--------------------------------------------------------------( 16 bytes )
	//--------------------------------------------------------------( 16 * 7 = 112 bytes )
#ifdef __cplusplus
	const static int REGISTER_SLOT = 10;
#endif // __cplusplus
};
#endif