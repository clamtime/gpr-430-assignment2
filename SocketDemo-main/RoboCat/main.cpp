#include <RoboCatPCH.h>
#include "SDL.h"
#include <SDL_image.h>
#undef main
#include "Unit.h"
#include "UnitManager.h"
#include <iostream>
#include <thread>
#include "PlayerUser.h"

PlayerUser* p1, * p2;

bool isRunning;
bool fullscreen;
Color color0, color1, color2;
Unit unit0, unit1, unit2;

void handleEvents();
void update(float dt);
void render();

#define TICK_INTERVAL    30

static Uint32 next_time;

int once = 0;

Uint32 time_left(void)
{
	Uint32 now;

	now = SDL_GetTicks();
	if (next_time <= now)
		return 0;
	else
		return next_time - now;
}

const std::string CLIENT_SEND_PORT = "1000", CLIENT_RECV_PORT = "2000";
//const std::string SERVER_SEND_PORT = "2000", SERVER_RECV_PORT = "1000";


#if _WIN32
int main(int argc, const char** argv)
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#else
const char** __argv;
int __argc;
int main(int argc, const char** argv, const char** argz)
{
	__argc = argc;
	__argv = argv;
	__argz = argz;
#endif

	SocketUtil::StaticInit();

	fullscreen = false;
	int flags = 0;
	flags = SDL_WINDOW_RESIZABLE;
	if (fullscreen) {
		flags = flags | SDL_WINDOW_FULLSCREEN;
	}
	
	p1 = new PlayerUser(flags, 1, 100);
	p2 = new PlayerUser(flags, 2, 1300);
	//serverPlayer = new PlayerUser(flags, 0, 1920/2-250);

	OutputWindow p1Client,p2Client;
	std::thread p1c([&p1Client]()
	{	
		p1->initTcpClient(CLIENT_SEND_PORT, CLIENT_RECV_PORT);
	});
	std::thread p2c([&p2Client]()
	{
		p2->initTcpClient(CLIENT_SEND_PORT, CLIENT_RECV_PORT);
	});


	Uint32 lastUpdate = SDL_GetTicks();

	// init colors
	color0 = Color(200, 75, 60, 255);
	color1 = Color(61, 226, 255, 255);
	color2 = Color(155, 132, 245, 255);


	int w, h;
	SDL_GetWindowSize(p1->window, &w, &h);
	p1->unitManager.createSquare(Vector2(w, h));
	p1->unitManager.createRectV(Vector2(w, h));
	p2->unitManager.createRectH(Vector2(w, h));

	isRunning = true;

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

	delete p1;
	delete p2;
	//delete serverPlayer;
	SocketUtil::CleanUp();

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
	SDL_SetRenderDrawColor(p1->renderer, 121, 121, 121, 255);
	SDL_RenderClear(p1->renderer);
	SDL_RenderPresent(p1->renderer);
	p1->unitManager.RenderUnits(p1->renderer);

	SDL_SetRenderDrawColor(p2->renderer, 121, 121, 121, 255);
	SDL_RenderClear(p2->renderer);
	SDL_RenderPresent(p2->renderer);
	p2->unitManager.RenderUnits(p2->renderer);
}

//simple update function
void update(float dt) 
{
	int w, h;
	SDL_GetWindowSize(p1->window, &w, &h);
	p1->unitManager.updateUnits(dt, Vector2(w, h));

	SDL_GetWindowSize(p2->window, &w, &h);
	p2->unitManager.updateUnits(dt, Vector2(w, h));


	once++;
	if (once == 100)
	{
		std::string msg(p1->packageUnitIntoString(p1->unitManager.units[0].getID()));
		p1->sendSocket->Send(msg.c_str(), msg.length());

		msg = p1->packageUnitIntoString(p1->unitManager.units[1].getID());
		p1->sendSocket->Send(msg.c_str(), msg.length());
	}
	else if (once == 120)
	{
		std::string msg = p1->packageUnitIntoString(p2->unitManager.units[0].getID());
		p2->recvSocket->Send(msg.c_str(), msg.length());
	}
}