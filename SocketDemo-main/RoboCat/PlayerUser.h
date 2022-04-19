#pragma once

#include "RoboCatPCH.h"
#include <SDL.h>
#include "UnitManager.h"

class PlayerUser
{
private:
	SDL_Renderer* renderer;
	SDL_Window* window;
	UnitManager unitManager;
	int playerNumber;

public:
	PlayerUser();
	PlayerUser(SDL_Renderer* _ren, SDL_Window* _win, UnitManager _um, int _flags, int _pnum, int _xpos);
	~PlayerUser();
};

