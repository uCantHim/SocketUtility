#include <iostream>
#include <thread>

#include "SUC.h"

int main()
{
	suc::ServerSocket server;
	server.bind(1234);
	auto client = server.accept();
	std::cout << "Client has connected.\n";

	client->sendString("Hi :D");
	client->close();
	auto response = client->recvString(suc::SUC_TIMEOUT_NEVER);
	server.close();


	std::cout << "\nPress any button to continue...\n";
	std::cin.get();
	return 0;
}