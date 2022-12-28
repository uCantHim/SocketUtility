#include <iostream>
#include <thread>

#include <suc/suc.h>

constexpr int PORT = 1234;

int main()
{
    std::vector<suc::ClientSocket> clients;
    int conCount = 0;

    suc::AsyncServer server(PORT, suc::AddressType::eIPv4);
    server.onConnection([&](suc::ClientSocket client) {
        std::vector<std::byte> data(100);
        for (auto& byte : data) {
            byte = std::byte{'A'};
        }
        client.send(data);

        std::cout << "Client has connected.\n";
        clients.push_back(std::move(client));
        conCount++;
    });
    server.onError([](auto& error) {
        std::cout << "An error occured in the server: " << error.what() << "\n";
    });
    server.onTerminate([]() {
        std::cout << "Server terminated.\n";
    });

    server.start();
    server.stop();
    server.start();

    while(conCount < 1);
    server.stop();

    return 0;
}
