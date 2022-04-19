#include "RoboCatPCH.h"
#include "PlayerUser.h"
#include <iostream>

PlayerUser::PlayerUser()
{
	renderer	= nullptr;
	window		= nullptr;
	unitManager = UnitManager();
}

PlayerUser::PlayerUser(SDL_Renderer* _ren, SDL_Window* _win, UnitManager _um, int _flags, int _pnum, int _xpos)
{
	playerNumber = _pnum;
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) 
	{
		std::cout << "Player " << playerNumber << " Subsystems Initialized!\n";

		std::string windowName = "Player" + std::to_string(playerNumber) + " Window";

		window = SDL_CreateWindow(windowName.c_str(), _xpos, SDL_WINDOWPOS_CENTERED, 500, 500, _flags);
		if (window) 
		{
			std::cout << "Window Created!\n";
			SDL_SetWindowMinimumSize(window, 100, 100);
		}
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (renderer) 
		{
			SDL_SetRenderDrawColor(renderer, 121, 121, 121, 255);
			std::cout << "Renderer created!\n";
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		}
	}
}

PlayerUser::~PlayerUser()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}