#include <iostream>
#include <thread>

#include "../SocketUtilityC++/SUC.h"

constexpr int PORT = 1234;

int main()
{
	suc::ServerSocket server;
	server.bind(PORT, suc::IPV4);

	std::vector<suc::ClientSocket> clients;

	std::thread([&]() {
		while (true)
		{
			auto client = server.accept();
			client->sendString("Hi :D");
			std::cout << "Client has connected.\n";
			clients.emplace_back(std::move(*client.release()));
		}
	}).detach();

	while(true);
	server.close();


	std::cout << "\nPress any button to continue...\n";
	std::cin.get();
	return 0;
}