#pragma once
#ifndef ASYNC_H
#define ASYNC_H

#include <functional>
#include <future>
#include <thread>

#include "ServerSocket.h"
#include "ClientSocket.h"

namespace suc
{
	template<typename... Args>
	using callback = std::function<void(Args...)>;


	class AsyncClient
	{
		AsyncClient(ClientSocket client);

	public:
		~AsyncClient() noexcept;

		static auto create(ClientSocket client) -> AsyncClient*;

		callback<ByteBuffer, ClientSocket*> onMessage{ [](ByteBuffer, ClientSocket*) {} };

	private:
		static void run(std::unique_ptr<AsyncClient> client);

		ClientSocket socket;
	};


	class AsyncServer
	{
		AsyncServer(int port, int family);

	public:
		~AsyncServer() noexcept;

		static auto create(
			int port,
			int family = SUC_IPV4,
			callback<AsyncClient*> onConnection = [](AsyncClient*) {}
		) -> AsyncServer*;

		void terminate();

		callback<AsyncClient*> onConnection;

	private:
		static void run(std::unique_ptr<AsyncServer> server);

		ServerSocket socket;
	};
}



#endif