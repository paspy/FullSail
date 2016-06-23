//--------------------------------------------------------------------------------
// Written by Justin Murphy
//--------------------------------------------------------------------------------
#include "pch.h"

#include "./MainWindow.h"
#include "./Log.h"
//--------------------------------------------------------------------------------
LRESULT CALLBACK GlobalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	LONG_PTR ObjPtr = GetWindowLongPtr(hWnd, 0);

	if (0 == ObjPtr) {
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	else {
		return reinterpret_cast<IWindProc *>(ObjPtr)->WindowProc(hWnd, uMsg, wParam, lParam);
	}
}
//--------------------------------------------------------------------------------
void CMainWindow::SetWindowed(bool _windowed) {
	if (nullptr == m_hWnd)
		return;

	if (!_windowed) {
		LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
		lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
		SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
		LONG lExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
		lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		SetWindowLong(m_hWnd, GWL_EXSTYLE, lExStyle);
		if (SetWindowPos(m_hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER | SWP_FRAMECHANGED) == 0) {
			LogError();
		}
		//Log("WINDOW full screen", "SysX ", GetSystemMetrics(SM_CXSCREEN), " SysY ", GetSystemMetrics(SM_CYSCREEN));
	}
	else {
		SetWindowLong(m_hWnd, GWL_STYLE, SavelStyle);
		SetWindowLong(m_hWnd, GWL_EXSTYLE, SavelExStyle);
		if (0 == SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, m_nWindowWidth, m_nWindowHeight, SWP_NOZORDER | SWP_FRAMECHANGED)) {
			LogError();
		}
		//Log("WINDOW Windoed", "SysX ", GetSystemMetrics(SM_CXSCREEN), " SysY ", GetSystemMetrics(SM_CYSCREEN));
	}
}
//--------------------------------------------------------------------------------
void CMainWindow::UpdateWindowState() {
	if (nullptr != m_hWnd) {
		RECT ClientRect;
		ClientRect.left = 0;
		ClientRect.top = 0;
		ClientRect.right = m_nWindowWidth;
		ClientRect.bottom = m_nWindowHeight;

		// Adjust the window size for correct device size
		RECT WindowRect = ClientRect;
		AdjustWindowRect(&WindowRect, m_dStyle, FALSE);

		int deltaX = (WindowRect.right - ClientRect.right) / 2;
		int deltaY = (WindowRect.bottom - ClientRect.bottom) / 2;

		MoveWindow(m_hWnd, deltaX, deltaY, m_nWindowWidth + deltaX * 2, m_nWindowHeight + deltaY * 2, true);
		//DebugFlags();
	}
}
//--------------------------------------------------------------------------------
void CMainWindow::SetSize(int width, int height) {
	
	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	if (rect.right == width && height == rect.bottom)
		return;

	m_nWindowWidth = width;
	m_nWindowHeight = height;

	UpdateWindowState();
}
//--------------------------------------------------------------------------------
CMainWindow::CMainWindow(int nWindowWidth,
	int nWindowHeight,
	const wchar_t* szWindowName) :
	m_nWindowWidth( nWindowWidth ),
	m_nWindowHeight( nWindowHeight ),
	m_hWnd( 0 ),
	m_dStyle(WS_BORDER | WS_VISIBLE ),
    m_sCaption(L"Full Sail")
{
	// Only one primary window should be created

	m_bIsActive = false;
	m_szWindowName = szWindowName;
}
//--------------------------------------------------------------------------------
CMainWindow::~CMainWindow() {
	//A thread cannot use DestroyWindow to destroy a window created by a different thread. 
	//
	//if (!DestroyWindow(m_hWnd)) {
	//	LogError();
	//}
	if (!UnregisterClass(m_szWindowName, m_hInstance)) {
		LogError();
	}
}
//--------------------------------------------------------------------------------
void CMainWindow::Initialize(IWindProc* _object) {
	WNDCLASSEX wc;

	// Setup the window class
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = GlobalWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(&GlobalWndProc);
	wc.hInstance = 0;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_szWindowName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	_object->BeforeRegisterWindowClass(wc);

	// Register the window class
	RegisterClassEx(&wc);

	// Create an instance of the window
	m_hWnd = CreateWindowEx(
		NULL,							// extended style
		wc.lpszClassName, 				// class name
		m_sCaption.c_str(),				// instance title
		m_dStyle,						// window style
		0, 0,							// initial x, y
		m_nWindowWidth,					// initial width
		m_nWindowHeight,				// initial height
		NULL,							// handle to parent 
		NULL,							// handle to menu
		NULL,							// instance of this application
		NULL);							// extra creation parms

	if (m_hWnd) {
		// Set in the "extra" bytes the pointer to the IWindowProc object
		// which handles messages for the window
		SetWindowLongPtr(m_hWnd, 0, reinterpret_cast<LONG_PTR>(_object));
		//SetWindowPos(m_hWnd, HWND_TOPMOST, m_iLeft, m_iTop, m_iWidth, m_iHeight, SWP_NOMOVE);
		SavelStyle = GetWindowLong(m_hWnd, GWL_STYLE);
		SavelExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);

		m_hInstance = wc.hInstance;
		Log("Created The windows window");
		return;
	}
	Log("error Creating the windows window");
}
//--------------------------------------------------------------------------------
void CMainWindow::DebugFlags() {
	return;
	LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	LONG lExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	Log("Style: ", lStyle, " EXstyle: ", lExStyle);
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	Log("Right ", rect.right, " Bottom ", rect.bottom, " top", rect.top, " left ", rect.left);
}
//--------------------------------------------------------------------------------
bool CMainWindow::HasMessages() {
	MSG msg;
	if( 0 == PeekMessage( &msg, 0, 0U, 0U, PM_NOREMOVE ) )
		return false;
	return true;
}
//--------------------------------------------------------------------------------
int CMainWindow::GetMaxHieght() const {
	return GetSystemMetrics(SM_CYSCREEN);
}
//--------------------------------------------------------------------------------
int CMainWindow::GetMaxWidth() const {
	return GetSystemMetrics(SM_CXSCREEN);
}
//--------------------------------------------------------------------------------
const HWND CMainWindow::GetHWnd() const {
	return m_hWnd;
}
//--------------------------------------------------------------------------------
bool CMainWindow::IsActive() { 
	return m_bIsActive; 
}
//--------------------------------------------------------------------------------
bool CMainWindow::MessagePump() {
	MSG msg;
	PeekMessage( &msg, 0, 0U, 0U, PM_REMOVE );

	if( WM_QUIT == msg.message )
		return false;

	TranslateMessage( &msg );
	DispatchMessage( &msg );

	return true;
}
//--------------------------------------------------------------------------------