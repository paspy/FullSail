// stdafx.cpp : source file that includes just the standard includes
// AssetManager.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

namespace FS_DEBUG {

    //*****************************************************************//
    // ALERT
    void Alert(const char* message) {
        // Print message to Output window
        Print(message);
        Print("\n");

        // Display a message box (using the active window & its title)
        char title[128];
        ::GetWindowTextA(::GetActiveWindow(), title, 128);
        ::MessageBoxA(::GetActiveWindow(), message, title, MB_OK | MB_ICONEXCLAMATION);
    }

    void Alert(const wchar_t* message) {
        // Print message to Output window
        Print(message);
        Print("\n");

        // Display a message box (using the active window & its title)
        wchar_t title[128];
        ::GetWindowTextW(::GetActiveWindow(), title, 128);
        ::MessageBoxW(::GetActiveWindow(), message, title, MB_OK | MB_ICONEXCLAMATION);
    }
    //*****************************************************************//

    //*****************************************************************//
    // ASSERT
    void Assert(bool expression, const char* message) {
        // Did the expression fail?
        if ( expression == false ) {
            // Display a message box
            Alert(message);

            // Trigger a breakpoint
            __debugbreak();				// USE THE CALLSTACK TO DEBUG!
        }
    }

    void Assert(bool expression, const wchar_t* message) {
        // Did the expression fail?
        if ( expression == false ) {
            // Display a message box
            Alert(message);

            // Trigger a breakpoint
            __debugbreak();				// USE THE CALLSTACK TO DEBUG!
        }
    }
    //*****************************************************************//

    //*****************************************************************//
    // PRINT
    void Print(const char* message) {
        // Print message to Output window
        ::OutputDebugStringA(message);
    }

    void Print(const wchar_t* message) {
        // Print message to Output window
        ::OutputDebugStringW(message);
    }

    //void Pritnt Output
    //*****************************************************************//
}
