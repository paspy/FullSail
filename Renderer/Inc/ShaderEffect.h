/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/
#pragma once


#include "RenderNode.h"
namespace Renderer
{
	class CRenderSet;
	class CShaderPass;
	class CShaderEffect : public IRenderNode
	{
	private:
		std::unique_ptr<CShaderPass> m_ShaderPass = nullptr;
        std::unique_ptr<CRenderSet>  m_Materials = nullptr;
	public:
		CShaderEffect();
		~CShaderEffect();

		virtual void Begin(IRenderNode* pCurrentView) final;
		virtual void End(IRenderNode* pCurrentView) final;

		void SetShaderPass(CShaderPass* _shaderPass); 

        inline CRenderSet* Materials() { return m_Materials.get(); }
		inline CShaderPass* GetShaderPass() { return m_ShaderPass.get(); }
	};
}
