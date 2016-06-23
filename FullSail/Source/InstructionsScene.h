/***************************************************************
|	File:		InstructionsScene.h
|	Author:		Jason N. Bloomfield
|	Created:	March 7, 2016
|	Modified:	March 18, 2016
|	Purpose:	InstructionsScene class used to store the instructions menu data
***************************************************************/
#pragma once

#include "IScene.h"


class InstructionsScene : public IScene {
public:
    InstructionsScene();
    ~InstructionsScene();


    void		Enter( void );
    void		Exit( void );

    bool		Input( void );
    void		Update( float _dt );
    void		Render( void );

    void		Input_Start( void ) { };
    void		Input_End( void ) { };

    bool		Input_Keyboard( void );
    bool		Input_Gamepad( void );

    void        LoadHudList();
    void        ReleaseHudList();

private:
    int		currSlide = 0;
    int	    currCursor = 0;
    float   scrollTimer = 0.0f;
    enum InstruItems { INTRODUCTION = 0, COMMANDS, BATTLES, ENEMEIES, RESOURCES, BACK };

    // GUI elements
    GUIMap m_hudElems;

};
