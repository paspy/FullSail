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

	class CDeviceResoureces
	{


		// Direct3D objects. 
		ID3D11Device*				m_d3dDevice;
		ID3D11DeviceContext*		m_d3dContext;
		IDXGISwapChain*				m_swapChain;
		

		std::wstring				m_deviceName;
		// Direct3D rendering objects. Required for 3D.
		ID3D11Texture2D*			m_d3dBackBuffer;
		ID3D11RenderTargetView*		m_d3dRenderTargetView;
		ID3D11DepthStencilView*		m_d3dDepthStencilView;
		D3D11_VIEWPORT				m_screenViewport;


		// Cached reference to the Window.
		HWND m_window;
		bool m_Windowed;
		IDXGIOutput*				m_pVideoOutput;
		// Cached device properties.
		D3D_FEATURE_LEVEL			m_d3dFeatureLevel;
		SIZE						m_d3dRenderTargetSize;
		SIZE						m_outputSize;

		float			m_Gamma;

		// Private Functions
		void CreateDeviceIndependentResources();
		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
	public:

		RENDERER_API ~CDeviceResoureces();

		RENDERER_API CDeviceResoureces(HWND mainWindow, unsigned int uwidth, unsigned int uheight, bool windowed = false, float gamma = 1.0f);
		RENDERER_API void ResizeWindow(unsigned int uwidth, unsigned int uheight);
		RENDERER_API void SetFullScreen(bool _fullScreen);
		RENDERER_API void SetGamma(float _gamma);

		// Default no vsync
		RENDERER_API void Present(unsigned int uvsync = 1);

		// The size of the render target, in pixels.
		RENDERER_API SIZE	GetOutputSize() const { return m_outputSize; }

		/***************
		* D3D Accessors.
		***************/
		RENDERER_API ID3D11Device*					GetD3DDevice() const { return m_d3dDevice; }
		RENDERER_API ID3D11DeviceContext*			GetD3DDeviceContext() const { return m_d3dContext; }
		RENDERER_API IDXGISwapChain*				GetSwapChain() const { return m_swapChain; }
		RENDERER_API D3D_FEATURE_LEVEL				GetDeviceFeatureLevel() const { return m_d3dFeatureLevel; }
		RENDERER_API ID3D11RenderTargetView*		GetBackBufferRenderTargetView() const { return m_d3dRenderTargetView; }
		RENDERER_API ID3D11Texture2D*				GetBackBuffer() const { return m_d3dBackBuffer; }
		RENDERER_API ID3D11DepthStencilView*		GetDepthStencilView() const { return m_d3dDepthStencilView; }
		RENDERER_API D3D11_VIEWPORT					GetScreenViewport() const { return m_screenViewport; }
		RENDERER_API bool                           GetWindowed(void) const { return m_Windowed; }

		RENDERER_API std::wstring                   GetDeviceName() const { return m_deviceName; }



	};

}