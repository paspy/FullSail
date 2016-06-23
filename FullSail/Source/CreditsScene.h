/***************************************************************
|	File:		CreditsScene.h
|	Author:		Jason N. Bloomfield
|	Created:	March 7, 2016
|	Modified:	April 8, 2016
|	Purpose:	CreditsScene class used to store the credits data
***************************************************************/
#pragma once

#include "IScene.h"


class CreditsScene : public IScene {
public:
    CreditsScene();
    ~CreditsScene() = default;


    void		Enter( void );
    void		Exit( void );

    bool		Input( void );
    void		Update( float _dt );
    void		Render( void );

    //SceneType	GetType	(void)	{ return SceneType::CREDITS; }

    void		Input_Start( void ) { }
    void		Input_End( void ) { }

    bool		Input_Keyboard( void );
    bool		Input_Gamepad( void );

    void        LoadHudList();
    void        ReleaseHudList();

private:
    GUIMap      m_hudElems;
    float       m_scrollPos;
    float       m_fg_y;
};
