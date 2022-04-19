#pragma once

#include "RoboCatPCH.h"
#include <SDL.h>
#include "UnitManager.h"

struct PlayerUser
{
	SDL_Renderer* renderer;
	SDL_Window* window;
	UnitManager unitManager;
	int playerNumber;
	std::string playerName;

	TCPSocketPtr sendSocket, recvSocket;

	PlayerUser();
	PlayerUser(SDL_Renderer* _ren, SDL_Window* _win, int _flags, int _pnum, int _xpos);
	~PlayerUser();

	void InitTcpClient(std::string sendPort, std::string recvPort);
	void InitTcpServer(std::string listenPort);
};

