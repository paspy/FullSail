#pragma once

//--------------------------------------------------------------------------------
inline void CApplication::Pause() { m_bIsActive = false; }
//--------------------------------------------------------------------------------
inline void CApplication::UnPause() { m_bIsActive = true; }
//--------------------------------------------------------------------------------
inline CApplication* CApplication::GetApplication() { return s_pInstance; }
//--------------------------------------------------------------------------------
// This must be defined by any client that uses this framework
CApplication*	CreateApplication(); // returns a pointer to a valid application object
//--------------------------------------------------------------------------------