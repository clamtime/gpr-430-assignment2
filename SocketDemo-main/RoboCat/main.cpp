#include <RoboCatPCH.h>
#include "SDL.h"
#include <SDL_image.h>
#undef main
#include <iostream>


SDL_Renderer* renderer;
SDL_Window* window;
bool isRunning;
bool fullscreen;
void handleEvents();
void update();
void render();
void loadImage(std::string IMG_PATH);

int main() 
{

	fullscreen = false;
	int flags = 0;
	flags = SDL_WINDOW_RESIZABLE;
	if (fullscreen) {
		flags = flags | SDL_WINDOW_FULLSCREEN;
	}
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Subsystems Initialized!\n";

		window = SDL_CreateWindow("Test Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, flags);
		if (window) {
			std::cout << "Window Created!\n";
			SDL_SetWindowMinimumSize(window, 100, 100);
		}
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (renderer) {
			SDL_SetRenderDrawColor(renderer, 121, 121, 121, 255);
			std::cout << "Renderer created!\n";
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			isRunning = true;
		}

	}

	Uint32 lastUpdate = SDL_GetTicks();

	while (isRunning) 
	{
		// for physics loop
		Uint32 current = SDL_GetTicks();
		float dt = (current - lastUpdate) / 1000.0f;

		handleEvents();
		update();
		render();

		lastUpdate = current;
	}

	//frees memory associated with renderer and window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();


	return 0;
}

void handleEvents() 
{
	SDL_Event event;
	SDL_PollEvent(&event);


	switch (event.type) 
	{
	case SDL_QUIT:
		isRunning = false;
		break;
	default:
		break;
	}
}

void render() 
{
	SDL_SetRenderDrawColor(renderer, 121, 121, 121, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	// render units here

}

//simple update function
void update() 
{

}