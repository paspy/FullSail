//--------------------------------------------------------------------------------
// CApplication
// Written By: Justin Murphy
//--------------------------------------------------------------------------------
#ifndef Application_h
#define Application_h
//--------------------------------------------------------------------------------
#include "./IWindProc.h"
#include "XTime.h"
//--------------------------------------------------------------------------------
namespace Renderer
{

class CRendererController;
}

namespace G2D {
    class CGraphics2D;
}
class CAssetManager;
class CMainWindow;
//--------------------------------------------------------------------------------
class CApplication :public IWindProc {
public:
	CApplication();
	virtual ~CApplication();

	// Init the application.  All user-level init should go here
	virtual bool Init(); 

	// Run the application.  This shouldn't need to change
	virtual void Run();

	// Handles all code that should be run each frame
	// timeDelta == amount of time (in seconds) since last call
	// timeTotal == amount of time (in seconds) since start
	virtual void DoFrame( float timeDelta, float timeTotal );
	// update but when window doesn't have focus
	virtual void DoIdleFrame( float timeDelta, float timeTotal );

	// Scene Init is called by Init.  This way a user-level class 
	// doesn't need to worry about setting up D3D or handling any 
	// errors during setup.
	virtual void SceneInit();
	virtual void SceneEnd();

	virtual void Pause();
	virtual void UnPause();
	LRESULT WindowProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam) override;
	static CApplication* GetApplication();
	
    void ShowDebugInfo();

	Renderer::CRendererController*  m_pRenderer;
	CAssetManager*                  m_pAssetManager;
    XTime                           Timer;
    float                           TimeScale;
	CMainWindow*                    m_window;
protected:
	static CApplication*	        s_pInstance;
	const wchar_t*	                m_szWindowTitle;
    int			                    m_nWindowWidth;
    int			                    m_nWindowHeight;
    bool		                    m_bIsActive;
    bool		                    m_bIsDone;

};
#include "Application.hpp"

#endif // Application_h
//--------------------------------------------------------------------------------