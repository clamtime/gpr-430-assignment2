#include <RoboCatPCH.h>
#include "SDL.h"
#include <SDL_image.h>
#include "Unit.h"
#undef main
#include <iostream>


SDL_Renderer* renderer;
SDL_Window* window;
bool isRunning;
bool fullscreen;
Color color0, color1, color2;
Unit unit0, unit1, unit2;

void handleEvents();
void update(float dt);
void render();
//void loadImage(std::string IMG_PATH);

#define TICK_INTERVAL    30

static Uint32 next_time;

Uint32 time_left(void)
{
	Uint32 now;

	now = SDL_GetTicks();
	if (next_time <= now)
		return 0;
	else
		return next_time - now;
}

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

	// init colors
	color0 = Color(200, 75, 60, 255);
	color1 = Color(61, 226, 255, 255);
	color2 = Color(155, 132, 245, 255);

	// init units
	unit0 = Unit(Vector2(100, 100), Vector2(75, 75), 0, color0);
	unit1 = Unit(Vector2(250, 250), Vector2(40, 70), 1, color1);
	unit2 = Unit(Vector2(50, 250), Vector2(70, 40), 2, color2);

	while (isRunning) 
	{
		// for physics loop
		Uint32 current = SDL_GetTicks();
		float dt = (current - lastUpdate) / 1000.0f;

		handleEvents();
		update(dt);
		render();

		SDL_Delay(time_left());
		next_time += TICK_INTERVAL;
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
	unit0.render(renderer);
	unit1.render(renderer);
	unit2.render(renderer);
}

//simple update function
void update(float dt) 
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	unit0.update(dt, Vector2(w, h));
	unit1.update(dt, Vector2(w, h));
	unit2.update(dt, Vector2(w, h));
}