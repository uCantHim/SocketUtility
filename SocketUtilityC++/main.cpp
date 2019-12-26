#include <cstdio>
#include <iostream>
#include <vector>
#include <chrono>
using namespace std::chrono;

#include "SUC.h"

int main()
{
	try
	{
		auto server = suc::ServerSocket(8080);
		auto client = server.accept();
		client->sendString("Hello! I will be your server today. What can I do for you?");
		auto answer = client->recvString();

		std::cout << "The client wrote: \"" << answer << "\"\n";

		std::cout << "Press any key to continue...\n";
		std::cin.get();
		server.close();
	}
	catch(const suc::suc_error& e)
	{
		std::cerr << e.getMsg() << '\n';
	}

	return 0;
}
