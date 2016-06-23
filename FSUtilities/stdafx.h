// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

// TODO: reference additional headers your program requires here
#include <string>
#include <vector>
#include <fstream>
#include <wrl.h>
#include <d3d11_1.h>
#include <SimpleMath.h>
#include <DirectXPackedVector.h>
#include <D3Dcompiler.h>
#include <DDSTextureLoader.h>

#define _FILE_HEDAER "FS TERRAIN"
#define _VERSION 0.1f

#ifndef SafeRelease
#define SafeRelease(x) { if(x){ x->Release(); x = nullptr; } }
#endif
#ifndef SafeDelete
#define SafeDelete(x) { if(x) {delete x; x = nullptr;} }
#endif

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x) {												\
		HRESULT hr = (x);									\
		if(FAILED(hr)) {									\
			LPWSTR output;									\
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |		\
				FORMAT_MESSAGE_IGNORE_INSERTS 	 |			\
				FORMAT_MESSAGE_ALLOCATE_BUFFER,				\
				NULL,										\
				hr,											\
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	\
				(LPTSTR) &output,							\
				0,											\
				NULL);										\
			MessageBox(NULL, output, L"HR Failed", MB_OK);  \
		}													\
	}
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

//namespace DX {
//    inline void ThrowIfFailed( HRESULT hr ) {
//        if ( FAILED( hr ) ) {
//            // Set a breakpoint on this line to catch DirectX API errors
//            throw std::exception();
//        }
//    }
//}