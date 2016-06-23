#pragma once
//--------------------------------------------------------------------------------
//__GOLD__BUILD__ takes out a lot of functionality
//If any of the next four defines get put in, some functionality returns
//THe fastest is to only have __GOLD__BUILD__ defined and the rest put a comment in front of the define
//#define __GOLD__BUILD__
//--------------------------------------------------------------------------------
//Below, these are the other 4 defines mentioned above
//--------------------------------------------------------------------------------
#define LOG_TO_FILE
//#define LOG_TO_CONSOLE
#define LOG_TO_VISUAL_STUDIO_OUTPUT
#define VISUAL_STUDIO_VISUAL_DEBUG_INFO
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

#include "./Log.hpp"
//--------------------------------------------------------------------------------
//Log file, and dump file got to the appdata/raoming folder
//To get there:
//Goto to windows run
//type %appdata%
//Look for folder name you supplied to the macro LogSetUp
//--------------------------------------------------------------------------------
//Only sets the object to nil if there are no references
#define SAFE_SHUT(obj) if(obj) { obj->ShutDown(); delete obj; obj = nullptr; }
//--------------------------------------------------------------------------------
//This define if to take out almost everything so the game can run faster
//This will still make a dump file and create the appdata folder
//n - Name of the game wide char * or std::wstring
// Creates a file in %appdata% where the log file and any dump files will be placed
//Should be one of the first things called
//This sets up dump files even when nothing is given always call first even with no defines
#define LogSetUp(gameName)	COMBINE(Log, __ID__)::InitDebugSystem(gameName)
//--------------------------------------------------------------------------------
//This define if to take out almost everything so the game can run faster
//This will still make a dump file and create the appdata folder
#ifndef __GOLD__BUILD__
	#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
		//Use instead of get last error for window calls
		//If there is any error it calls log
		#define LogError()		COMBINE(Log, __ID__)::LogLastError()
		//Send in any number of arguments to get logged to a file, the console window and the Output window
		//Use Log("something", vector.size(), argument)
		//If there is a linker error or an error where the compiler cannot find the function add the overload to Log.hpp
		#define Log(...)		COMBINE(Log, __ID__)::print(##__VA_ARGS__)
		//Clean-up the debugger
		//Call at the end of the program
		#define LogShutDown()		COMBINE(Log, __ID__)::Close()
		//m - msg from wndproc
		//Used to log what order of messages are sent in wndproc
		#define LogWndProc(msg)	COMBINE(Log, __ID__)::CheckMessage(msg)
		//m - Wparam from wndproc
		//Used to log the system messages (in wndproc), another case off WM_SYSKEYDOWN messages
		//Only valid in the secondary case, is used on the main switch or another message it will print unknown or incorrect infomration
		#define LogSystem(wParam)	COMBINE(Log, __ID__)::CheckSystemMessage(wParam)
	#endif //#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
	#if !defined(LOG_TO_CONSOLE) && !defined(LOG_TO_FILE) && !defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
		#define LogError() __noop
		#define Log(...) __noop
		#define LogShutDown() __noop
		#define LogWndProc __noop
		#define LogSystem __noop
	#endif

#else //!__GOLD__BUILD__
	#define LogWndProc __noop
	#define LogError() __noop
#if defined(LOG_TO_CONSOLE) || defined(LOG_TO_FILE) || defined(LOG_TO_VISUAL_STUDIO_OUTPUT)
	#define Log(...) COMBINE(Log, __ID__)::print(##__VA_ARGS__)
#else
	#define Log(...) __noop
#endif
	#define LogShutDown() __noop
	#define LogSystem __noop
#endif //__GOLD__BUILD__