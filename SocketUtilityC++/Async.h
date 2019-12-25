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
		explicit AsyncClient(std::unique_ptr<ClientSocket> client);

		AsyncClient(AsyncClient&&) noexcept = default;
		AsyncClient& operator=(AsyncClient&&) noexcept = default;

	public:
		AsyncClient(const AsyncClient&) = delete;
		AsyncClient& operator=(const AsyncClient&) = delete;

		/*
		Creates an asynchronous client from a ClientSocket.
		Basically detaches the passed client connection from the current thread. */
		static auto create(std::unique_ptr<ClientSocket> client) -> AsyncClient*;

		/*
		Returns the unique ID of the client. */
		[[nodiscard]]
		auto getId() const noexcept -> size_t;

		/*
		Called when the client receives a message. */
		callback<const ByteBuffer&, ClientSocket*> onMessage;
		/*
		Calles when the client's thread terminates.
		This may happen when the connection is closed remotely or an error occurs
		in the socket. */
		callback<AsyncClient*> onTerminate;

	private:
		/*
		AsyncClient::run() holds the ownership of the actual client.
		This works because AsyncClient::create() is the only way to create new
		AsyncClients. */
		static void run(std::unique_ptr<AsyncClient> client);

		std::unique_ptr<ClientSocket> socket;
		size_t id;
	};


	class AsyncServer
	{
		AsyncServer(int port, int family);
		AsyncServer(AsyncServer&&) noexcept = default;
		AsyncServer& operator=(AsyncServer&&) noexcept = default;

	public:
		AsyncServer(const AsyncServer&) = delete;
		AsyncServer& operator=(const AsyncServer&) = delete;
		~AsyncServer() noexcept;

		static auto create(
			int port,
			int family = SUC_IPV4,
			callback<AsyncClient*> onConnection = [](AsyncClient*) {}
		) -> AsyncServer*;

		void terminate();

		/*
		Called when a new client connects to the server. */
		callback<AsyncClient*> onConnection;

	private:
		static void run(std::unique_ptr<AsyncServer> server);

		ServerSocket socket;
	};
} // namespace suc



#endif