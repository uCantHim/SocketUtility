#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <memory>

#include "SocketUtility.h"

// TODO:
// Create a custom address structure that encapsulates the #ifdef hacks

namespace suc
{
	class ClientSocket;

	/* +++ ServerSocket +++
	A TCP server socket. */
	class ServerSocket
	{
	public:
		ServerSocket() noexcept = default;
		explicit ServerSocket(int port, int family = IPV4);

		ServerSocket(const ServerSocket&) = delete;
		ServerSocket(ServerSocket&&) noexcept = default;

		ServerSocket& operator=(const ServerSocket&) = delete;
		ServerSocket& operator=(ServerSocket&&) noexcept = default;

		~ServerSocket() noexcept;

		/* +++ bind() +++
		Initializes the server.
		- ARG family: Must be either IPV4 or IPV6. */
		void bind(int port, int family = IPV4);

		/* +++ accept() +++
		Waits for an incoming connection. Returns a ClientSocket when a client successfully connects to the ServerSocket.
		This blocks the thread until a client has connected to the socket. */
		[[nodiscard]]
		auto accept() const -> std::unique_ptr<ClientSocket>;

		/* +++ close() +++
		Closes the server socket. */
		void close();

		[[nodiscard]]
		bool isClosed() const noexcept;

	private:
		bool _isClosed{ true };
		SOCKET socket{ INVALID_SOCKET };

#ifdef OS_IS_WINDOWS
		addrinfo* address{ nullptr };
#endif

#ifdef OS_IS_LINUX
		sockaddr_in address{};
#endif
	};
} // namespace suc



#endif
