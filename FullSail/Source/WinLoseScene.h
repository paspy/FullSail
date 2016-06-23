/***************************************************************
|	File:		WinScene.h
|	Author:		Jason N. Bloomfield
|	Created:	March 16, 2016
|	Modified:	March 18, 2016
|	Purpose:	WinScene class used to store the win scene data
***************************************************************/
#pragma once

#include "IScene.h"

#define ANIM_TIME 4.0f

class WinLoseScene : public IScene {
public:
    WinLoseScene();
    ~WinLoseScene() = default;

    void		Enter( void );
    void		Exit( void );

    bool		Input( void );
    void		Update( float _dt );
    void        AnimationUpdate( float _dt );
    void        StatisticsUpdate(bool toggle);
    void		Render( void );

    void		Input_Start( void ) { }
    void		Input_End( void ) { }

    bool		Input_Keyboard( void );
    bool		Input_Gamepad( void );

    void        LoadHudList();
    void        ReleaseHudList();


    bool        IsVictory() const { return m_isVictory; }
    void        IsVictory( bool val ) { m_isVictory = val; }
private:
    enum WinLoseItems { SCREEN_SHOT = 0, CONTINUE };
    enum WinLoseAnimStage {
        SHOW_STATS = 0,
        SHOW_SKULL,
        SHOW_WIN_LOSE_TEXT,
        SHOW_SCORE,
        ANIM_END
    };
    int     currCursor;
    float   scrollTimer;
    GUIMap  m_hudElems;
    std::wstringstream m_wss;
    std::wstring m_wsFinalScore;
    WinLoseAnimStage m_currStage;
    bool    m_isVictory;
    bool    m_screenShotSaved;
    bool    m_disableInput;
};
