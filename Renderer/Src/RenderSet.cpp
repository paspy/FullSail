/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/
#include "stdafx.h"
#include "..\Inc\RenderSet.h"
#include "..\Inc\RenderNode.h"

namespace Renderer
{
	

	CRenderSet::CRenderSet()
	{
	}


	CRenderSet::~CRenderSet()
	{
	}

	void CRenderSet::AddtoHead(IRenderNode* node)
	{
		
		m_Set.push_front(node);
		m_Set.unique();

	}
	void CRenderSet::AddtoTail(IRenderNode* node)
	{
		m_Set.push_back(node);
		m_Set.unique();
	}
	void CRenderSet::Sort(CompareFunc sortMethod)
	{
		m_Set.sort(sortMethod);
	}
}