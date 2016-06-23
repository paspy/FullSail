#ifndef stdafx_h__
#define stdafx_h__

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <map>
#include <allocators>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <filesystem>

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


#endif // stdafx_h__