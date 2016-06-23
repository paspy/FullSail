// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>


// TODO: reference additional headers your program requires here
#include <exception>
#include <memory>
#include <stdexcept>
#include <cassert>
#include <DirectXColors.h>
#include <DDSTextureLoader.h>


namespace DX {
    inline void ThrowIfFailed ( HRESULT hr ) {
        if ( FAILED ( hr ) ) {
            // Set a breakpoint on this line to catch DirectX API errors
            throw std::exception ();
        }
    }
}