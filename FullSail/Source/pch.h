#ifndef pch_h__
#define pch_h__

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h> // for proper mouse coordinates when working with dual monitors
#include <objbase.h>
#include <wrl.h>
#include <wrl/client.h>
#include <float.h>

#include <DirectXMath.h>
#include <SimpleMath.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <list>
#include <string>
#include <algorithm>
#include <cassert>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <functional>

// DirectXTK_Desktop_2015 includes
#include <Keyboard.h>
#include <GamePad.h>
#include <ScreenGrab.h>

// custom static libs
#include <tinyxml2.h>

// custom dynamic libs
#include <RendererController.h>
#include <AssetManager.h>
#include <Graphics2D.h>
#include <MemoryManager.h>
#include <Material.h>
#include <Renderable.h>
#include <Utilities.h>
#include <Particle.h>
#include <C3DHUD.h>
#include <Miscellaneous.h>
#include "fsEventManager.h"

//Alerts
#define EVENT_ALERT "Alert"

// FS debug Assertion

#if defined( DEBUG ) || defined( _DEBUG )
#define FS_ASSERT( expression, message )   FS_DEBUG::Assert( expression, message )
#define FS_PRINT( message )                FS_DEBUG::Print( message )

#define FS_PRINT_OUTPUT( s )            \
{   		\
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}
#undef _HAS_ITERATOR_DEBUGGING
#define _HAS_ITERATOR_DEBUGGING 0
#else 
#define FS_ASSERT( expression, message )    (void)0
#define FS_PRINT( message )                 (void)0
#define FS_PRINT_OUTPUT( s )            \
{   		\
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}

#endif

#define SafeRelease(x)  { if(x){ x->Release(); x = nullptr; } }
#define SafeDelete(x)   { if(x) {delete x; x = nullptr; }}

namespace FS_DEBUG {
	//*****************************************************************//
	// Alert
	//	- displays a message box and prints to the Output window
	void Alert(const char* message);
	void Alert(const wchar_t* message);

	//*****************************************************************//
	// Assert
	//	- displays a message box and triggers a breakpoint
	//	  only if the expression is false
	void Assert(bool expression, const char* message);
	void Assert(bool expression, const wchar_t* message);

	//*****************************************************************//
	// Print
	//	- prints a message to the Output window
	void Print(const char* message);
	void Print(const wchar_t* message);

}


#endif // pch_h__
