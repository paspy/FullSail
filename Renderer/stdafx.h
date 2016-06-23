// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <comdef.h>
#include <objbase.h>
// DirectX Header Files:
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <SimpleMath.h>
// STL Header Files:
#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <list>
#include <unordered_map>
#include <ctime>
#include <algorithm>
#include <random>
#include <string>

// Custom headers
#include <Miscellaneous.h>
#include <MemoryManager.h>
#include <tinyxml2.h>


#define SAFE_RELEASE(com) { int keeper = 0; if(com) { if (0 == (keeper = com->Release() ) ) com = nullptr; } }


#ifdef _DEBUG
	// Check for SDK Layer support.
inline bool SdkLayersAvailable()
{
	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_NULL,
		0,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		nullptr,
		nullptr,
		nullptr
		);

	return SUCCEEDED(hr);
}

#define SetD3DName(object, name)								\
					{															\
						if(name)												\
							object->SetPrivateData(WKPDID_D3DDebugObjectName,	\
								(UINT)strlen( name ), name );					\
					}


#else
#define SetD3DName(object, name) 
#endif


inline void AssertIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		_com_error err(hr);
		OutputDebugString(err.ErrorMessage());
		assert(false);
	}
}