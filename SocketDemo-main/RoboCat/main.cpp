#include <RoboCatPCH.h>
#include "SDL.h"
#include <SDL_image.h>
#include "Unit.h"
#undef main
#include <iostream>
#include <thread>

const std::string SENDPORT = "8000", RECVPORT = "9000";
//TCPSocketPtr sendSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET), 
//		     recvSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

SDL_Renderer* rendererP1, *rendererP2, *rendererServer;
SDL_Window* windowP1, *windowP2, *windowServer;
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

//void initServer()
//{
//	// Create socket
//	if (recvSocket == nullptr)
//	{
//		SocketUtil::ReportError("Creating listening socket");
//		
//	}
//
//	recvSocket->SetNonBlockingMode(true);
//
//	LOG("%s", "Listening socket created");
//
//	// Bind() - "Bind" socket -> tells OS we want to use a specific address
//
//	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
//	if (listenAddress == nullptr)
//	{
//		SocketUtil::ReportError("Creating listen address");
//		
//	}
//
//	if (recvSocket->Bind(*listenAddress) != NO_ERROR)
//	{
//		SocketUtil::ReportError("Binding listening socket");
//		// This doesn't block!
//		
//	}
//
//	LOG("%s", "Bound listening socket");
//
//	// Blocking function call -> Waits for some input; halts the program until something "interesting" happens
//	// Non-Blocking function call -> Returns right away, as soon as the action is completed
//
//	// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
//	if (recvSocket->Listen() != NO_ERROR)
//	{
//		SocketUtil::ReportError("Listening on listening socket");
//		
//	}
//
//	LOG("%s", "Listening on socket");
//
//	// Accept() - Accept on socket -> Blocking; Waits for incoming connection and completes TCP handshake
//
//	LOG("%s", "Waiting to accept connections...");
//	SocketAddress incomingAddress;
//	TCPSocketPtr connSocket = recvSocket->Accept(incomingAddress);
//	while (connSocket == nullptr)
//	{
//		connSocket = recvSocket->Accept(incomingAddress);
//		// SocketUtil::ReportError("Accepting connection");
//		// 
//	}
//
//	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());
//
//	char buffer[4096];
//	int32_t bytesReceived = connSocket->Receive(buffer, 4096);
//	while (bytesReceived < 0)
//	{
//		bytesReceived = connSocket->Receive(buffer, 4096);
//	}
//	std::string receivedMsg(buffer, bytesReceived);
//	LOG("Received message from %s: %s", incomingAddress.ToString().c_str(), receivedMsg.c_str());
//}
//
//
//void initClient()
//{
//	// Create socket
//	if (sendSocket == nullptr)
//	{
//		SocketUtil::ReportError("Creating client socket");
//	}
//
//	LOG("%s", "Client socket created");
//
//	// Bind() - "Bind" socket -> tells OS we want to use a specific address
//
//	std::string address = StringUtils::Sprintf("127.0.0.1:%s", PORT.c_str());
//	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
//	if (clientAddress == nullptr)
//	{
//		SocketUtil::ReportError("Creating client address");
//		
//	}
//
//	if (sendSocket->Bind(*clientAddress) != NO_ERROR)
//	{
//		SocketUtil::ReportError("Binding client socket");
//		// This doesn't block!
//		
//	}
//
//	LOG("%s", "Bound client socket");
//
//	// Connect() -> Connect socket to remote host
//
//	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
//	if (servAddress == nullptr)
//	{
//		SocketUtil::ReportError("Creating server address");
//		
//	}
//
//	if (sendSocket->Connect(*servAddress) != NO_ERROR)
//	{
//		SocketUtil::ReportError("Connecting to server");
//		
//	}
//
//	LOG("%s", "Connected to server!");
//}

void DoTcpServer()
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

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
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
		}
		});

	std::cout << "Press enter to exit at any time!\n";
	std::cin.get();
	quit = true;
	connSocket->~TCPSocket(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it!
	receiveThread.join();
}

void DoTcpClient(std::string port)
{
	// Create socket
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
	}

	LOG("%s", "Client socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	std::string address = StringUtils::Sprintf("127.0.0.1:%s", port.c_str());
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating client address");
		ExitProcess(1);
	}

	if (clientSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding client socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");

	// Connect() -> Connect socket to remote host

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	if (clientSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}

	LOG("%s", "Connected to server!");

	while (true)
	{
		LOG("%s", "Client sending message");
		std::string msg("Hello server! How are you?");
		clientSocket->Send(msg.c_str(), msg.length());
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

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
	OutputWindow servWin, clientWin;
	//std::thread st([&servWin]()
	//	{
	//		DoTcpServer();
	//	});
	//std::thread ct([&servWin]()
	//	{
	//		DoTcpClient(SENDPORT);
	//	});

	
	fullscreen = false;
	int flags = 0;
	flags = SDL_WINDOW_RESIZABLE;
	if (fullscreen) {
		flags = flags | SDL_WINDOW_FULLSCREEN;
	}
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Subsystems Initialized!\n";

		windowP1 = SDL_CreateWindow("P1 Window", 100, SDL_WINDOWPOS_CENTERED, 500, 500, flags);
		if (windowP1) {
			std::cout << "Window Created!\n";
			SDL_SetWindowMinimumSize(windowP1, 100, 100);
		}
		rendererP1 = SDL_CreateRenderer(windowP1, -1, SDL_RENDERER_ACCELERATED);
		if (rendererP1) {
			SDL_SetRenderDrawColor(rendererP1, 121, 121, 121, 255);
			std::cout << "Renderer created!\n";
			SDL_SetRenderDrawBlendMode(rendererP1, SDL_BLENDMODE_BLEND);
			isRunning = true;
		}

		windowP2 = SDL_CreateWindow("P2 Window", 1300, SDL_WINDOWPOS_CENTERED, 500, 500, flags);
		if (windowP2) {
			std::cout << "Window Created!\n";
			SDL_SetWindowMinimumSize(windowP2, 100, 100);
		}
		rendererP2 = SDL_CreateRenderer(windowP2, -1, SDL_RENDERER_ACCELERATED);
		if (rendererP2) {
			SDL_SetRenderDrawColor(rendererP2, 121, 121, 121, 255);
			std::cout << "Renderer created!\n";
			SDL_SetRenderDrawBlendMode(rendererP2, SDL_BLENDMODE_BLEND);
			isRunning = true;
		}

		windowServer = SDL_CreateWindow("Server Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, flags);
		if (windowServer) {
			std::cout << "Window Created!\n";
			SDL_SetWindowMinimumSize(windowServer, 100, 100);
		}
		rendererServer = SDL_CreateRenderer(windowServer, -1, SDL_RENDERER_ACCELERATED);
		if (rendererServer) {
			SDL_SetRenderDrawColor(rendererServer, 121, 121, 121, 255);
			std::cout << "Renderer created!\n";
			SDL_SetRenderDrawBlendMode(rendererServer, SDL_BLENDMODE_BLEND);
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
	SDL_DestroyRenderer(rendererP1);
	SDL_DestroyRenderer(rendererP2);
	SDL_DestroyRenderer(rendererServer);
	SDL_DestroyWindow(windowP1);
	SDL_DestroyWindow(windowP2);
	SDL_DestroyWindow(windowServer);
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
	SDL_SetRenderDrawColor(rendererServer, 121, 121, 121, 255);
	SDL_RenderClear(rendererServer);
	SDL_RenderPresent(rendererServer);

	// render units here
	unit0.render(rendererServer);
	unit1.render(rendererServer);
	unit2.render(rendererServer);
}

//simple update function
void update(float dt) 
{
	int w, h;
	SDL_GetWindowSize(windowServer, &w, &h);

	std::string msg("Hello server! DT is: " + std::to_string(dt));
	//sendSocket->Send(msg.c_str(), msg.length());
	std::this_thread::sleep_for(std::chrono::seconds(1));

	unit0.update(dt, Vector2(w, h));
	unit1.update(dt, Vector2(w, h));
	unit2.update(dt, Vector2(w, h));
}