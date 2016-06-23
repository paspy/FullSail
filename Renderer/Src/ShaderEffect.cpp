/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/
#include "stdafx.h"
#include "..\Inc\ShaderEffect.h"
#include "..\Inc\View.h"
#include "..\Inc\RenderSet.h"
#include "..\Inc\ShaderPass.h"

namespace Renderer
{
	CShaderEffect::CShaderEffect() : m_ShaderPass(nullptr), m_Materials(new CRenderSet)
	{
	}


	CShaderEffect::~CShaderEffect()
	{

	}

	void CShaderEffect::SetShaderPass(CShaderPass* _shaderPass)
	{
		m_ShaderPass = std::unique_ptr<CShaderPass>(_shaderPass);
	}

	/*virtual*/ void CShaderEffect::Begin(IRenderNode* pCurrentView) /*final*/
	{
		CView& view = (CView&)(*pCurrentView);
		view.m_CurrentShaderEffect = this;
		view.m_CurrentState = SHADEREFFECT_BEGIN;
		m_ShaderPass->Begin(pCurrentView);

	}
	/*virtual*/ void CShaderEffect::End(IRenderNode* pCurrentView) /*final*/
	{
		CView& view = (CView&)(*pCurrentView);
		view.m_CurrentState = SHADEREFFECT_END;
		m_ShaderPass->End(pCurrentView);
		view.m_CurrentShaderEffect = nullptr;

	}
}