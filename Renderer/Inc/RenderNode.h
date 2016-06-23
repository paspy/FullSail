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
#include "SharedDefines.h"


namespace Renderer
{
	enum eRenderState : unsigned int
	{
		NULLSTATE = 0, VIEW_BEGIN, VIEW_END, SHADEREFFECT_BEGIN, SHADEREFFECT_END, SHADERPASS_BEGIN, SHADERPASS_END, MATERIAL_BEGIN, MATERIAL_END, RENDERING

	};

	class IRenderNode
	{
	public:
		virtual void Begin(IRenderNode* pCurrentView) = 0;
		virtual void End(IRenderNode* pCurrentView) = 0;
		IRenderNode() {};
		virtual ~IRenderNode() {};
	};
}
