#include <RoboCatPCH.h>
#include "SDL.h"
#include <SDL_image.h>
#undef main
#include <iostream>


//don't do this, this is just an example
SDL_Renderer* renderer;
SDL_Window* window;
bool isRunning;
bool fullscreen;
void handleEvents();
void update();
void render();
void loadImage(std::string IMG_PATH);

//please don't put all your code in main like I did.
int main() {

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

	while (isRunning) {
		handleEvents();
		update();
		render();
	}

	//frees memory associated with renderer and window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);	//error here
	SDL_Quit();


	return 0;
}

//handles any events that SDL noticed.
void handleEvents() {
	//the only event we'll check is the  SDL_QUIT event.
	SDL_Event event;
	SDL_PollEvent(&event);


	switch (event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	default:
		break;
	}
}

//simple render function
void render() {
	SDL_SetRenderDrawColor(renderer, 121, 121, 121, 255);
	SDL_RenderClear(renderer);

	//your stuff to render would typically go here.
	SDL_RenderPresent(renderer);
}

//simple update function
void update() {
	//if things could update the code would go in here.
}


void loadImage(std::string IMG_PATH)
{
	//SDL_Texture* img = NULL;

	//// load our image
	//img = IMG_LoadTexture(renderer, IMG_PATH);
	//SDL_QueryTexture(img, NULL, NULL, &w, &h); // get the width and height of the texture
	//// put the location where we want the texture to be drawn into a rectangle
	//// I'm also scaling the texture 2x simply by setting the width and height
	//SDL_Rect texr; texr.x = WIDTH / 2; texr.y = HEIGHT / 2; texr.w = w * 2; texr.h = h * 2;
}