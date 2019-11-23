#include <cstdio>
#include <iostream>
#include <vector>
#include <chrono>
using namespace std::chrono;

#include "SUC.h"
#include "Async.h"

int main()
{
	auto server = suc::AsyncServer::create(1234);
	int clientID = 1;
	server->onConnection = [&clientID](suc::AsyncClient* newClient) {
		std::cout << "A client has connected :D\n";
		int id = clientID++;
		newClient->onMessage = [id](suc::ByteBuffer buf, suc::ClientSocket* client) {
			std::cout << "Message received from client " << id << ": " << buf.to<char*>() << "\n";
			client->sendString("Hello!");
		};
	};



	std::cout << "Press any key to continue...\n";
	std::cin.get();
	server->terminate();

	std::cout << "Press any key to continue...\n";
	std::cin.get();
	return 0;
}
