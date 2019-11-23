#include <cstdio>
#include <iostream>
#include <vector>
#include <chrono>
using namespace std::chrono;

#include "SUC.h"
#include "HttpServer.h"

int main()
{
	suc::HttpServer server(1234);


	std::cout << "Press any key to continue...\n";
	std::cin.get();
	return 0;
}
