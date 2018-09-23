#ifndef __PLAYER_INPUT_
#define __PLAYER_INPUT_

#include <SDL.h>
#include "cfunctor.h"

#define SET_KEY_UP_TRUE          playerInput->currKeys |= 0x01
#define SET_KEY_UP_FALSE         playerInput->currKeys &= 0xFE

#define SET_KEY_DOWN_TRUE        playerInput->currKeys |= 0x02
#define SET_KEY_DOWN_FALSE       playerInput->currKeys &= 0xFD

#define SET_KEY_RIGHT_TRUE       playerInput->currKeys |= 0x04
#define SET_KEY_RIGHT_FALSE      playerInput->currKeys &= 0xFB

#define SET_KEY_LEFT_TRUE        playerInput->currKeys |= 0x08
#define SET_KEY_LEFT_FALSE       playerInput->currKeys &= 0xF7

#define SET_RUNNING_TRUE		 playerInput->currKeys |= 0x10
#define SET_RUNNING_FALSE		 playerInput->currKeys &= 0xEF

#define IS_KEY_UP_PRESSED     (playerInput->currKeys & 0x01)
#define IS_KEY_DOWN_PRESSED   (playerInput->currKeys & 0x02)
#define IS_KEY_RIGHT_PRESSED  (playerInput->currKeys & 0x04)
#define IS_KEY_LEFT_PRESSED   (playerInput->currKeys & 0x08)
#define IS_RUNNING_PRESSED	  (playerInput->currKeys & 0x10)

struct tPlayerInput
{
	char currKeys;
	int mouseX;
	int mouseY;
	int screenWidth;
	int screenHeight;
	bool newMousePos;
	
    CFunctor * setKeyUpTrueFunctor;
    CFunctor * setKeyUpFalseFunctor;
    CFunctor * setKeyDownTrueFunctor;
    CFunctor * setKeyDownFalseFunctor;
    CFunctor * setKeyRightTrueFunctor;
    CFunctor * setKeyRightFalseFunctor;
    CFunctor * setKeyLeftTrueFunctor;
    CFunctor * setKeyLeftFalseFunctor;
    CFunctor * setKeyRunningTrueFunctor;
    CFunctor * setKeyRunningFalseFunctor;
    
    CFunctor * mouseMotionFunctor;
    
	tPlayerInput(int _screenWidth, int _screenHeight) : currKeys(0), mouseX(0), mouseY(0), screenWidth(_screenWidth), screenHeight(_screenHeight), newMousePos(false),
		
		setKeyUpTrueFunctor(new CSpecificFunctor<tPlayerInput>(this, &tPlayerInput::setKeyUpTrue)),
		setKeyUpFalseFunctor(new CSpecificFunctor<tPlayerInput>(this, &tPlayerInput::setKeyUpFalse)),
		setKeyDownTrueFunctor(new CSpecificFunctor<tPlayerInput>(this, &tPlayerInput::setKeyDownTrue)),
		setKeyDownFalseFunctor(new CSpecificFunctor<tPlayerInput>(this, &tPlayerInput::setKeyDownFalse)),
		setKeyRightTrueFunctor(new CSpecificFunctor<tPlayerInput>(this, &tPlayerInput::setKeyRightTrue)),
		setKeyRightFalseFunctor(new CSpecificFunctor<tPlayerInput>(this, &tPlayerInput::setKeyRightFalse)),
		setKeyLeftTrueFunctor(new CSpecificFunctor<tPlayerInput>(this, &tPlayerInput::setKeyLeftTrue)),
		setKeyLeftFalseFunctor(new CSpecificFunctor<tPlayerInput>(this, &tPlayerInput::setKeyLeftFalse)),
		setKeyRunningTrueFunctor(new CSpecificFunctor<tPlayerInput>(this, &tPlayerInput::setKeyRunningTrue)),
		setKeyRunningFalseFunctor(new CSpecificFunctor<tPlayerInput>(this, &tPlayerInput::setKeyRunningFalse)),
		mouseMotionFunctor(new CSpecificFunctor<tPlayerInput>(this,&tPlayerInput::defaultMouseMotion))
	{
	}
	
	void setKeyUpTrue()       { currKeys |= 0x01 ;}
	void setKeyUpFalse()      { currKeys &= 0xFE ;}
	void setKeyDownTrue()     { currKeys |= 0x02 ;}
	void setKeyDownFalse()    { currKeys &= 0xFD ;}
	void setKeyRightTrue()    { currKeys |= 0x04 ;}
	void setKeyRightFalse()   { currKeys &= 0xFB ;}
	void setKeyLeftTrue()     { currKeys |= 0x08 ;}
	void setKeyLeftFalse()    { currKeys &= 0xF7 ;}
	void setKeyRunningTrue()  { currKeys |= 0x10 ;}
	void setKeyRunningFalse() { currKeys &= 0xEF ;}
	
	void defaultMouseMotion()
	{
		int tmp[2];

		SDL_GetMouseState (&tmp[0], &tmp[1]);
	
		if ((tmp[0] - (screenWidth >> 1))
			|| (tmp[1] - (screenHeight >> 1)))
		{
			newMousePos = true;
	
			mouseX =
				tmp[0] - (screenWidth >> 1);
			mouseY =
				tmp[1] - (screenHeight >> 1);
	
			// Set the mouse position to the middle of our window
			SDL_WarpMouse (screenWidth >> 1	// This is a binary shift to get half the width
					, screenHeight >> 1	// This is a binary shift to get half the height
					);
		}
	}
};


#endif
