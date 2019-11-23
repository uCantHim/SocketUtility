#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include <memory>

#include "SocketUtility.h"

namespace suc
{
	class SucClientSocket;

	/* +++ SucServerSocket +++
	A TCP server socket. */
	class SucServerSocket
	{
	public:
		SucServerSocket() = default;
		explicit SucServerSocket(int port, int family = SUC_IPV4);

		SucServerSocket(const SucServerSocket&) = delete;
		SucServerSocket& operator=(const SucServerSocket&) = delete;
		SucServerSocket(SucServerSocket&&) noexcept = default;
		SucServerSocket& operator=(SucServerSocket&&) noexcept = default;

		~SucServerSocket() noexcept;

		/* +++ bind() +++
		Initializes the server.
		- ARG family: Must be either SUC_IPV4 or SUC_IPV6. */
		void bind(int port, int family = SUC_IPV4);

		/* +++ accept() +++
		Waits for an incoming connection. Returns a ClientSocket when a client successfully connects to the ServerSocket.
		This blocks the thread until a client has connected to the socket. */
		[[nodiscard]] std::unique_ptr<SucClientSocket> accept() const;

		/* +++ close() +++
		Closes the server socket. */
		void close();

		[[nodiscard]] bool isClosed() const noexcept;

	private:
		bool _isClosed{ true };

		SOCKET socket{ INVALID_SOCKET };
		addrinfo* address{ nullptr };
	};
} // namespace suc



#endif
