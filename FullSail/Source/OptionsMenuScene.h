/***************************************************************
|	File:		OptionsMenuScene.h
|	Author:		Jason N. Bloomfield
|	Created:	March 7, 2016
|	Modified:	March 17, 2016
|	Purpose:	OptionsMenuScene class used to store the options menu data
***************************************************************/
#pragma once

#include "IScene.h"


class OptionsMenuScene : public IScene {
public:
    OptionsMenuScene();
    ~OptionsMenuScene();


    void		Enter( void );
    void		Exit( void );

    bool		Input( void );
    void		Update( float _dt );
    void		Render( void );

    //SceneType	GetType	(void)	{ return SceneType::OPTIONS; }

    void		Input_Start( void ) { }
    void		Input_End( void ) { }

    bool		Input_Keyboard( void );
    bool		Input_Gamepad( void );

    void        ApplyOption();

    void        LoadHudList();
    void        ReleaseHudList();

private:
    // Options Menu selection
    enum OptionItems { UNKNOWN = -1, VOLUME_MUSIC = 0, VOLUME_SFX, GAMMA, RESOLUTION, CONTROLLER, ACCEPT, BACK, DEFAULT };
    int	    currCursor = 0;
    int     m_bOptionFlag;
    float   scrollTimer = 0.0f;

    float m_gammaValue;

    GUIMap  m_hudElems;

};
