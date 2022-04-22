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
	std::string SEPERATOR_TOKEN = "!";


	TCPSocketPtr sendSocket, recvSocket, recvConnSocket;

	PlayerUser();
	PlayerUser(int _flags, int _pnum, int _xpos);
	~PlayerUser();

	void initTcpClient(std::string sendPort, std::string recvPort);
	void initTcpServer(std::string listenPort);

	std::string packageUnitIntoString(int _id);
	void decodeUnitString(std::string _unitString, bool onlyPrint = false);
	void recieveNewUnit(int _id, int _type, Vector2 _pos, Vector2 _size, Color _col);
};