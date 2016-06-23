/***************************************************************
|	File:		PauseMenuScene.h
|	Author:		Jason N. Bloomfield
|	Created:	March 7, 2016
|	Modified:	March 18, 2016
|	Purpose:	PauseMenuScene class used to store the pause menu data
***************************************************************/
#pragma once

#include "IScene.h"


class PauseMenuScene : public IScene {
public:
    PauseMenuScene();
    ~PauseMenuScene();


    void		Enter( void );
    void		Exit( void );

    bool		Input( void );
    void		Update( float _dt );
    void		Render( void );

    //SceneType	GetType	(void)	{ return SceneType::PAUSE; }

    void		Input_Start( void ) { }
    void		Input_End( void ) { }

    bool		Input_Keyboard( void );
    bool		Input_Gamepad( void );

    void        LoadHudList();
    void        ReleaseHudList();

private:

    // Pause Menu selection
    enum PauseItems { UNKNOWN = -1, RESUME = 0, INSTRUNCTION, OPTIONS, EXIT, NUM_CHOICES };
    int				currCursor = 0;
    float			scrollTimer = 0.0f;

    // GUI elements
    GUIMap m_hudElems;
    std::wstringstream m_wss;

};
