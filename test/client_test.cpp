#include <iostream>

#include <suc/suc.h>

constexpr int PORT = 1234;

int main()
{
    try
    {
        suc::ClientSocket client;
        client.connect(suc::addr::localhost, PORT, suc::AddressType::eIPv4);

        std::cout << "Waiting for server message...\n";
        auto msg = client.recvString(1024, suc::Timeout::never());
        std::cout << "Received message from server: " << msg << "\n";
        std::cout << "Message was " << msg.size() << " bytes long.\n";

        std::cout << "\nPress any button to continue...\n";
        std::cin.get();
        client.send("Hi. I'm leaving, goodbye good sir.");
        client.close();
    }
    catch (const suc::Error& e)
    {
        std::cerr << e.what() << "\n";
    }

    return 0;
}
