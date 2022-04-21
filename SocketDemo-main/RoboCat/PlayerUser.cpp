#include "RoboCatPCH.h"
#include "PlayerUser.h"
#include <iostream>
#include <thread>

// FUNCTION FROM user704565 AT https://stackoverflow.com/a/16286297:
std::vector<std::string> split(std::string str, std::string sep) {
	char* cstr = const_cast<char*>(str.c_str());
	char* current;
	std::vector<std::string> arr;
	current = strtok(cstr, sep.c_str());
	while (current != NULL) {
		arr.push_back(current);
		current = strtok(NULL, sep.c_str());
	}
	return arr;
}


PlayerUser::PlayerUser()
{
	renderer	= nullptr;
	window		= nullptr;
	unitManager = UnitManager();
}

PlayerUser::PlayerUser(int _flags, int _pnum, int _xpos)
{
	unitManager = UnitManager();
	sendSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	recvSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

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

void PlayerUser::initTcpClient(std::string sendPort, std::string recvPort)

{
	// Create socket
	sendSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (sendSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
	}

	LOG("%s", "Client socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	std::string address = StringUtils::Sprintf("127.0.0.1:%s", sendPort.c_str());
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating client address");
		ExitProcess(1);
	}

	if (sendSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding client socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");

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

	LOG("%s", "Connected to server!");
}

void PlayerUser::initTcpServer(std::string listenPort)
{
	// Create socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	//listenSocket->SetNonBlockingMode(true);

	LOG("%s", "Listening socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:"+ listenPort);
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError("Creating listen address");
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
			if (receivedMsg[0] == '$')
			{
				decodeUnitString(receivedMsg.erase(0, 1));
			}
		}
		});

	std::cout << "Press enter to exit at any time!\n";
	std::cin.get();
	quit = true;
	connSocket->~TCPSocket(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it!
	receiveThread.join();
}

std::string PlayerUser::packageUnitIntoString(int _id)
{
	std::string toReturn = "$";
	
	Unit* temp = unitManager.getUnit(_id);
	if (temp != nullptr)
	{
		Vector2 pos = temp->getPosition();
		Vector2 size = temp->getSize();
		Color col = temp->getColor();

		toReturn +=
			std::to_string(temp->getID())   + SEPERATOR_TOKEN +
			std::to_string(temp->getType()) + SEPERATOR_TOKEN +
			std::to_string(pos.x)  + SEPERATOR_TOKEN +
			std::to_string(pos.y)  + SEPERATOR_TOKEN +
			std::to_string(size.x) + SEPERATOR_TOKEN +
			std::to_string(size.y) + SEPERATOR_TOKEN +
			std::to_string(col.r)  + SEPERATOR_TOKEN +
			std::to_string(col.g)  + SEPERATOR_TOKEN +
			std::to_string(col.b)  + SEPERATOR_TOKEN +
			std::to_string(col.a);
	}

	return toReturn;
}

void PlayerUser::decodeUnitString(std::string _unitString, bool onlyPrint)
{
	if (_unitString.length() == 0)
		return;

	vector<std::string> splitUnitString;

	if (_unitString[0] == '$')
		_unitString = _unitString.erase(0, 1);
	splitUnitString = split(_unitString, SEPERATOR_TOKEN);

	int _id = std::stoi(splitUnitString[0]);
	int _type = std::stoi(splitUnitString[1]);
	Vector2 _pos = Vector2(std::stoi(splitUnitString[2]), std::stoi(splitUnitString[3]));
	Vector2 _size = Vector2(std::stoi(splitUnitString[4]), std::stoi(splitUnitString[5]));
	Color _col = Color(
		(Uint8)std::stoi(splitUnitString[6]), 
		(Uint8)std::stoi(splitUnitString[7]), 
		(Uint8)std::stoi(splitUnitString[8]), 
		(Uint8)std::stoi(splitUnitString[9]));
	
	if (!onlyPrint)
		recieveNewUnit(_id, _type, _pos, _size, _col);
	else
	{
		std::string str = "ID: " + std::to_string(_id) + '\n' + 
			"Type: " + std::to_string(_type) + '\n' +
			"Position: " + _pos.ToString() + '\n' +
			"Size: " + _size.ToString() + '\n' +
			"Color: " + _col.ToString() +'\n';

		std::cout << str;
	}
}

void PlayerUser::recieveNewUnit(int _id, int _type, Vector2 _pos, Vector2 _size, Color _col)
{
	// ensure unit does not exist locally
	if (unitManager.getUnit(_id) == nullptr)
	{
		unitManager.createReceivedUnit(_pos, _size, _col, _type, _id);
	}
}