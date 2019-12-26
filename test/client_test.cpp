#include <iostream>

#include "../SocketUtilityC++/SUC.h"

constexpr int PORT = 8080;

int main()
{
	try
	{
		suc::ClientSocket client;
		client.connect("192.168.0.42", PORT);
		auto msg = client.recvString(suc::TIMEOUT_NEVER);
		std::cout << "Received message from server: " << msg << "\n";
		client.sendString("Hi. I'm leaving, goodbye good sir.");
		client.close();
	}
	catch (const suc::suc_error& e)
	{
		std::cerr << e.what();
	}

	std::cout << "\nPress any button to continue...\n";
	std::cin.get();
	return 0;
}
