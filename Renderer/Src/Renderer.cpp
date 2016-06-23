/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/

#include "stdafx.h"
#include "..\stdafx.h"
#include "..\Inc\Renderer.h"
#include "..\Inc\StepTimer.h"

namespace Renderer
{
	CRenderer::CRenderer(const std::shared_ptr<CDeviceResoureces>& deviceResources) :
		m_deviceResources(deviceResources)
	{
	}


	void CRenderer::Update(StepTimer const& timer)
	{

	}

	void CRenderer::Render()
	{
		m_deviceResources->GetD3DDeviceContext()->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::Aquamarine);
		m_deviceResources->Present();
	}
}