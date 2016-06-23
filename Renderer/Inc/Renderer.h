/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/
#pragma once
#ifdef RENDERER_EXPORTS
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllimport)
#endif

#include "DeviceResoureces.h"

namespace Renderer
{
	class StepTimer;
	class CRenderer
	{
		 std::shared_ptr<CDeviceResoureces> m_deviceResources;
	public:
		RENDERER_API CRenderer(const std::shared_ptr<CDeviceResoureces>& deviceResources);
		RENDERER_API void Update(StepTimer const& timer);
		RENDERER_API void Render();
	};

}