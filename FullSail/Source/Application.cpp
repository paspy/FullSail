//--------------------------------------------------------------------------------
// Written by Justin Murphy
//--------------------------------------------------------------------------------
#include "pch.h"
//#include <vld.h>
#include "Application.h"
#include "MainWindow.h"
#include "Log.h"
#include "XTime.h"
#include "Game.h"
#include "GameplayScene.h"
#include "GameSetting.h"

//--------------------------------------------------------------------------------
CApplication* CApplication::s_pInstance = nullptr;
//--------------------------------------------------------------------------------
int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,// -debug
	_In_ int nShowCmd) {
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nShowCmd);

	LogSetUp(L"FULLSAIL");

	// init memory manager first
	FSMM::MemoryManager::GetInstance();

    // init game setting class
    GameSetting::GetRef();

	//Seed rand
	srand((unsigned)time(0));
	CApplication* pApp = nullptr;

	pApp = CreateApplication();

	if (!pApp->Init()) {
		return 1;
	}
	pApp->SceneInit();
	pApp->Run();

	pApp->SceneEnd();

	ShowCursor(true);
	delete pApp;


	return 0;
}
//--------------------------------------------------------------------------------
CApplication::CApplication() :
	m_szWindowTitle(L"Default window name"),
	TimeScale(1.0f),

	m_bIsActive(true),
	m_pRenderer(nullptr) {

	if (s_pInstance)
	{
		Log("Will Not open Application already running");
		return;//throw CError("Application object already created!\n");
	}
	s_pInstance = this;
}
//--------------------------------------------------------------------------------
CApplication::~CApplication() {
	//CInputManager::DestroyInstance();
	delete m_window;
	m_window = nullptr;
	// Release before the device release!!!
	G2D::CGraphics2D::GetRef().Destory();

	m_pAssetManager->Shutdown();
	if (m_pRenderer) {
		delete m_pRenderer;
		m_pRenderer = nullptr;
	}

}
//--------------------------------------------------------------------------------
bool CApplication::Init() {
    auto& gameSetting = GameSetting::GetRef();
    // load game setting
    if ( !gameSetting.LoadExistingSetting() ) {
        gameSetting.GenerateDefaultSetting();
        gameSetting.SaveCurrentSetting();
    }

#if !_DEBUG
    switch ( gameSetting.Resolution() ) {
    case GameSetting::eRES_1280x720:
        m_nWindowWidth = 1280;
        m_nWindowHeight = 720;
        break;
    case GameSetting::eRES_1920x1080:
        m_nWindowWidth = 1920;
        m_nWindowHeight = 1080;
        break;
    }
    
#else
    m_nWindowWidth = 1280;
    m_nWindowHeight = 720;

#endif

	m_window = new CMainWindow(m_nWindowWidth, m_nWindowHeight, m_szWindowTitle);
	m_window->Initialize(this);
	ShowCursor(false);

#if _DEBUG
	m_window->SetWindowed(true);
	m_pRenderer = new Renderer::CRendererController(m_window->GetHWnd(), m_window->GetWindowWidth(), m_window->GetWindowHeight(),true);
#else
    if ( m_nWindowWidth == 1280 ) {
        m_window->SetWindowed( true );
        m_pRenderer = new Renderer::CRendererController( m_window->GetHWnd(), m_window->GetWindowWidth(), m_window->GetWindowHeight(), true );
    } else {
        m_window->SetWindowed( false );
        m_pRenderer = new Renderer::CRendererController( m_window->GetHWnd(), m_window->GetWindowWidth(), m_window->GetWindowHeight(), false, GameSetting::GetRef().GammaValue() );
    }
	
#endif

	m_pAssetManager = &CAssetManager::GetRef();
	m_pAssetManager->SetDevicePtr(m_pRenderer->m_deviceResources->GetD3DDevice());
	m_pAssetManager->Init();

	return true;
}
//--------------------------------------------------------------------------------
void CApplication::ShowDebugInfo() {
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	static float totalTime = 0.0f;
	frameCnt++;

	if ((Timer.TotalTime() - timeElapsed) >= 1.0f) {
		std::wostringstream outs;
		auto deviceName = m_pRenderer->m_deviceResources->GetDeviceName();
		auto fsmm = FSMM::MemoryManager::GetInstance();
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;
		outs.precision(6);
		outs << m_window->Caption()
			<< L" - " << deviceName
			<< L" - FPS: " << fps
			<< L" Time: " << mspf << L" (ms)";
//#ifdef _DEBUG
		outs.precision(4);
		float mmprecent = ((float)fsmm->GetTotalMemoryUsed() / (float)fsmm->GetTotalMemory()) * 100.0f;
		outs << L" - Pool Used: " << mmprecent << L"%"
			<< " # of Alloc: " << fsmm->GetNumberOfAllocations();
//#endif // _DEBUG

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
		SetWindowText(m_window->GetHWnd(), outs.str().c_str());
	}

}
//--------------------------------------------------------------------------------
void CApplication::Run() {

	while (true) {
		// Message pump
		while (m_window->HasMessages()) {
			if (false == m_window->MessagePump())
				return;
		}

        Timer.Signal();
        if ( m_bIsActive ) {
            DoFrame( float( Timer.Delta() )*TimeScale, float( Timer.TotalTime() )*TimeScale );
        } else {
            DoIdleFrame( float( Timer.Delta() )*TimeScale, float( Timer.TotalTime() )*TimeScale );
        }
		m_pRenderer->m_deviceResources->Present();
        ShowDebugInfo();

	}
}
//--------------------------------------------------------------------------------
void CApplication::DoFrame(float timeDelta, float timeTotal) {
    if ( m_pRenderer ) {
        m_pRenderer->SetTotalTime( (float)Timer.TotalTime() );
    }
}
//--------------------------------------------------------------------------------
void CApplication::DoIdleFrame(float timeDelta, float timeTotal) { }
//--------------------------------------------------------------------------------
LRESULT CApplication::WindowProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam) {

	// DirectX keyboard input
	DirectX::Keyboard::ProcessMessage(_msg, _wParam, _lParam);
	CGame*							game = (CGame*)CApplication::GetApplication();
	IScene*							currscene = nullptr;	//sm->GetCurrScene();
	Keyboard::KeyboardStateTracker*	kbst = nullptr;

	switch (_msg)
	{
		//case WM_CANCELMODE: ???
	case WM_ACTIVATE:
		if (LOWORD(_wParam) == WA_INACTIVE)
		{
			currscene = game->GetSceneManager()->GetCurrScene();

#if !_DEBUG
			if (currscene->GetType() == IScene::SceneType::GAMEPLAY)
				((GameplayScene*)currscene)->GoToPauseScene();
#endif

		}
		break;

	case WM_ACTIVATEAPP:
	case WM_IME_SETCONTEXT:
	case WM_KILLFOCUS:
	case WM_NCACTIVATE:
	case WM_SETFOCUS:
		if (game != nullptr)
		{
			kbst = game->GetKeyboardTracker();
			if (kbst != nullptr)
				kbst->Reset();
		}
		break;
	default:
		break;
	}




	switch (_msg) {
	case WM_NCACTIVATE:
	{

	} break;
	case WM_SETFOCUS:
	{

	} break;
	//to remove the beep from alt tabbing from fullscreen to windowed
	case WM_SYSCOMMAND:
	{
		switch (_wParam) {
		case SC_KEYMENU:
		{
			return 1;
		}
		};
	} break;
	case WM_SYSKEYDOWN:
	{
		switch (_wParam) {
		case VK_RETURN:
		{
			if ((_lParam & (1 << 29)) != 0) {
				if (m_pRenderer)
				{
					m_window->SetWindowed(!m_pRenderer->m_deviceResources->GetWindowed());
					m_pRenderer->m_deviceResources->ResizeWindow(m_window->GetWindowWidth(), m_window->GetWindowHeight());
					m_pRenderer->m_deviceResources->SetFullScreen(m_pRenderer->m_deviceResources->GetWindowed());
				}
			}
		}break;
		};
	} break;
	case WM_SIZE:
	{
		//m_window->DebugFlags();
		
		if (m_pRenderer)
		{
			m_window->SetSize(m_window->GetWindowWidth(), m_window->GetWindowHeight());
			m_pRenderer->m_deviceResources->ResizeWindow(m_window->GetWindowWidth(), m_window->GetWindowHeight());
		}
	} break;
	// This message is sent when a window has been destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return(0);
	} break;
	};

	return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
}
//--------------------------------------------------------------------------------
void CApplication::SceneInit() { }
//--------------------------------------------------------------------------------
void CApplication::SceneEnd() {

}

