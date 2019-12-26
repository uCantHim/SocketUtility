#include <iostream>
#include <thread>

#include "../SocketUtilityC++/SUC.h"

int main()
{
	suc::ServerSocket server;
	server.bind(8080);
	auto client = server.accept();
	std::cout << "Client has connected.\n";

	client->sendString("Hi :D");
	client->close();
	auto response = client->recvString(suc::TIMEOUT_NEVER);
	server.close();


	std::cout << "\nPress any button to continue...\n";
	std::cin.get();
	return 0;
}