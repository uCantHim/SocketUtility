#include "Async.h"

#include <iostream>



// ------------------------ //
//		Client class		//
// ------------------------ //

suc::AsyncClient::AsyncClient(ClientSocket client)
	:
	socket(std::move(client))
{
}


suc::AsyncClient::~AsyncClient() noexcept
{
	std::cout << "Async client destroyed.\n";
}


auto suc::AsyncClient::create(ClientSocket client) -> AsyncClient*
{
	auto newClient = std::unique_ptr<AsyncClient>(new AsyncClient(std::move(client)));
	AsyncClient* result = newClient.get();
	std::thread{ &AsyncClient::run, std::move(newClient) }.detach();

	return result;
}


void suc::AsyncClient::run(std::unique_ptr<AsyncClient> client)
{
	auto& socket = client->socket;
	while (true)
	{
		try {
			auto received = std::move(socket.recv(SUC_TIMEOUT_NEVER).value());
			client->onMessage(std::move(received), &socket);

			if (socket.isClosed()) {
				std::cout << "Socket closed.\n";
				break;
			}
		}
		catch (const SucException& err) {
			std::cout << "Exception in AsyncClient: " << err.getMsg()
				<< ". Stopping client.\n";
			break;
		}
	}

	std::cout << "Async client terminated.\n";
}



// ------------------------ //
//		Server class		//
// ------------------------ //

suc::AsyncServer::AsyncServer(int port, int family)
	:
	socket(port, family)
{
}


suc::AsyncServer::~AsyncServer() noexcept
{
	std::cout << "AsyncServer destroyed.\n";
}


auto suc::AsyncServer::create(int port, int family, callback<AsyncClient*> onConnection) -> AsyncServer*
{
	auto newServer = std::unique_ptr<AsyncServer>(new AsyncServer(port, family));
	newServer->onConnection = onConnection;
	AsyncServer* result = newServer.get();
	std::thread{ &AsyncServer::run, std::move(newServer) }.detach();

	return result;
}


void suc::AsyncServer::terminate()
{
	socket.close();
}


void suc::AsyncServer::run(std::unique_ptr<AsyncServer> server)
{
	auto& socket = server->socket;
	while (!socket.isClosed())
	{
		try {
			auto newClient = AsyncClient::create(std::move(*socket.accept().release()));
			server->onConnection(newClient);
		}
		catch (const SucException& err) {
			std::cout << "Exception in AsyncServer: " << err.getMsg()
				<< ". Stopping server.\n";
			break;
		}
	}
}
