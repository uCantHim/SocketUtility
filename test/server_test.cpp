#include <iostream>

#include "../SocketUtilityC++/SUC.h"

constexpr int PORT = 8080;

int main()
{
	suc::ServerSocket server;
	server.bind(PORT);
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