/***************************************************************
|	File:		IntroScene.h
|	Author:		Jason N. Bloomfield
|	Created:	March 16, 2016
|	Modified:	March 17, 2016
|	Purpose:	IntroScene class used to store the intro scene data
***************************************************************/
#pragma once

#include "IScene.h"

#define INIT_TIME	1.0f
#define PAGE_TIME	3.0f

class IntroScene : public IScene {
public:
    IntroScene();
    ~IntroScene();

    void		Enter( void );
    void		Exit( void );

    bool		Input( void );
    void		Update( float _dt );
    void		Render( void );

    void		Input_Start( void ) { }
    void		Input_End( void ) { }

    bool		Input_Keyboard( void );
    bool		Input_Gamepad( void );


    bool		PostUpdate( float _dt );

    void        LoadHudList();
    void        ReleaseHudList();

private:

    GUIMap           m_hudElems;
    GUIMap::iterator m_hudIter;
    // Menu elements
    int				m_nCurrPage = 0;
    float			m_fPageTimer = 0.0f;
    float           m_fFadeInColor;
    float           m_fFadeOutColor;

};
