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

#include "..\stdafx.h"

namespace Renderer
{
	class IRenderNode;
	class CRenderSet
	{
		typedef  bool(*CompareFunc)(const IRenderNode* const lhs, const IRenderNode* const  rhs);
	public:
		std::list<IRenderNode*> m_Set;
		RENDERER_API CRenderSet();
		RENDERER_API ~CRenderSet();
		RENDERER_API void AddtoHead(IRenderNode* node);
		RENDERER_API void AddtoTail(IRenderNode* node);
		RENDERER_API void Sort(CompareFunc sortMethod);
	};
}
