#include "RoboCatPCH.h"
#include "PlayerUser.h"
#include <iostream>
#include <thread>

PlayerUser::PlayerUser()
{
	renderer	= nullptr;
	window		= nullptr;
	unitManager = UnitManager();
}

PlayerUser::PlayerUser(SDL_Renderer* _ren, SDL_Window* _win, int _flags, int _pnum, int _xpos)
{
	unitManager = UnitManager();

	playerNumber = _pnum;
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) 
	{
		playerName = _pnum == 0 ? "Server" : "Player" + std::to_string(playerNumber);
		std::cout << playerName << " client "<< " Subsystems Initialized!\n";

		std::string windowName = playerName + " Window";

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

void PlayerUser::InitTcpClient(std::string sendPort, std::string recvPort)

{
	// Create socket
	sendSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (sendSocket == nullptr)
	{
		SocketUtil::ReportError(("Creating " + playerName + " client socket").c_str());
		ExitProcess(1);
	}

	LOG("%s", "Client " + playerName + " socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	std::string address = StringUtils::Sprintf("127.0.0.1:%s", sendPort.c_str());
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError(("Creating " + playerName + " client address").c_str());
		ExitProcess(1);
	}

	if (sendSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError(("Binding " + playerName + " client socket").c_str());
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound " + playerName + " client socket");

	// Connect() -> Connect socket to remote host

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:"+recvPort);
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	if (sendSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}

	LOG("%s", playerName + " Connected to server!");
}

void PlayerUser::InitTcpServer(std::string listenPort)
{
	// Create socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError(("Creating " + playerName + " listening socket").c_str());
		ExitProcess(1);
	}

	//listenSocket->SetNonBlockingMode(true);

	LOG("%s", "Listening socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:"+ listenPort);
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError(("Creating " + playerName + " listen address").c_str());
		ExitProcess(1);
	}

	if (listenSocket->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound listening socket");

	// Blocking function call -> Waits for some input; halts the program until something "interesting" happens
	// Non-Blocking function call -> Returns right away, as soon as the action is completed

	// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		ExitProcess(1);
	}

	LOG("%s", "Listening on socket");

	// Accept() - Accept on socket -> Blocking; Waits for incoming connection and completes TCP handshake

	LOG("%s", "Waiting to accept connections...");
	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
	while (connSocket == nullptr)
	{
		connSocket = listenSocket->Accept(incomingAddress);
		// SocketUtil::ReportError("Accepting connection");
		// ExitProcess(1);
	}

	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());

	bool quit = false;
	std::thread receiveThread([&]() { // don't use [&] :)
		while (!quit) // Need to add a quit here to have it really exit!
		{
			char buffer[4096];
			int32_t bytesReceived = connSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				// handle disconnect
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Receiving");
				return;
			}

			std::string receivedMsg(buffer, bytesReceived);
			LOG("Received message from %s: %s", incomingAddress.ToString().c_str(), receivedMsg.c_str());
		}
		});

	std::cout << "Press enter to exit at any time!\n";
	std::cin.get();
	quit = true;
	connSocket->~TCPSocket(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it!
	receiveThread.join();
}
