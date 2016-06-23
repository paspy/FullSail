#pragma once
#include "pch.h"

#include "./Interface.h"
//--------------------------------------------------------------------------------
CREATE_INTERFACE(IWindProc) {
	//Used to handle input and windows resizes
	//other classes that derive off application need to class super::WindowProc to jeep the functionality
	virtual LRESULT WindowProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam) = 0;
	//Used to adjust WNDCLASSEX to special icons cursors etc
	virtual void BeforeRegisterWindowClass(WNDCLASSEX &_wc) {}
};
//--------------------------------------------------------------------------------