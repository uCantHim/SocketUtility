#include <stdio.h>
#include <iostream>
#include <vector>

#include "SUC.h"

int main()
{
	suc::ClientSocket client;
	client.connect(suc::SUC_ADDR_LOCAL_HOST, 1234);
	auto msg = client.recvString(suc::SUC_TIMEOUT_NEVER);
	std::cout << "Received message from server: " << msg << "\n";
	client.close();




	std::cout << "\nPress any button to continue...\n";
	std::cin.get();
	return 0;
}