#pragma once
#include "Camera.h"
class CinematicCamera :
	public Camera
{
public:
	CinematicCamera();
	virtual ~CinematicCamera();
	virtual void Update(float dt);
};

