#include <stdio.h>
#include <iostream>
#include <vector>

#include "../SocketUtilityC++/SUC.h"

int main()
{
	try
	{
		suc::ClientSocket client;
		client.connect("192.168.0.42", 8080);
		auto msg = client.recvString(suc::TIMEOUT_NEVER);
		std::cout << "Received message from server: " << msg << "\n";
		client.sendString("Hi. I'm leaving, goodbye good sir.");
		client.close();
	}
	catch (const suc::suc_error& e)
	{
		std::cerr << e.getMsg();
	}

	std::cout << "\nPress any button to continue...\n";
	std::cin.get();
	return 0;
}
