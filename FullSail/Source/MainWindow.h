//--------------------------------------------------------------------------------
// MainWindow
// Written By: Justin Murphy
//
// Handles window creation, class registration, message processing, and updating the application
//--------------------------------------------------------------------------------
#ifndef MainWindow_h
#define MainWindow_h
//--------------------------------------------------------------------------------
#include "pch.h"

#include "./IWindProc.h"
//--------------------------------------------------------------------------------
class CMainWindow {
public:
	CMainWindow(int nWindowWidth, int nWindowHeight, const wchar_t* szWindowName = L"MurphyGaming Tutorial Series");
	~CMainWindow();

	void Initialize(IWindProc* _object);
	const HWND GetHWnd() const;
	bool IsActive();
	bool HasMessages();
	// True -> continue running
	// False -> got a WM_QUIT message. Stop the application.
	bool MessagePump();

	int GetWindowWidth() const;
	int GetWindowHeight() const;
	int GetMaxHieght() const;
	int GetMaxWidth() const;
	void SetWindowed(bool _windowed);
	void SetSize(int width, int height);
	void UpdateWindowState();
	void DebugFlags();
    std::wstring Caption() const { return m_sCaption; }
    void Caption( std::wstring val ) { m_sCaption = val; }
private:
	HWND				m_hWnd;
	HINSTANCE			m_hInstance;
	const wchar_t*		m_szWindowName;
	int					m_nWindowWidth;
	int					m_nWindowHeight;
	bool				m_bIsActive;
	DWORD				m_dStyle;
	std::wstring		m_sCaption;
	LONG				SavelStyle;
	LONG				SavelExStyle;

};
//--------------------------------------------------------------------------------
inline int CMainWindow::GetWindowWidth() const { return m_nWindowWidth; }
//--------------------------------------------------------------------------------
inline int CMainWindow::GetWindowHeight() const { return m_nWindowHeight; }
//--------------------------------------------------------------------------------
#endif // MainWindow_h
