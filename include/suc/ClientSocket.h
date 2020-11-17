#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <string>

#include "SocketUtility.h"

namespace suc
{
	class ClientSocket
	{
	public:
		ClientSocket() = default;
		explicit ClientSocket(SOCKET socket) noexcept;

		ClientSocket(const ClientSocket&) = delete;
		ClientSocket(ClientSocket&& other) noexcept;
		~ClientSocket() noexcept;

		ClientSocket& operator=(const ClientSocket&) = delete;
		ClientSocket& operator=(ClientSocket&& rhs) noexcept;

		/* +++ connect() +++
		Attempts to connect to a server.
		- ARG ip: Specifies the server's IP address. If this is an empty string, the socket will
		instead use the local host address.
		- ARG port: Specifies the server's port.
		- ARG family: Must be either IPV4, IPV6 or IPVX.
		- RETURN: Returns true if a connection could be established, false otherwise. */
		/**
		 * Attempt to connect to a remote server.
		 * 
		 * @param std::string ip The server's IP address. An empty string or the string "localhost"
		 * is interpreted as the localhost address.
		 * @param int port The server's port
		 * @param int family: Must be either IPV4, IPV6 or IPVX. WARNING: IPv6 currently not supported.
		 * 
		 * @return bool True if the connection was successfully established, false otherwise.
		 * 
		 * @throw suc_error
		 */
		bool connect(std::string ip, int port, int family = IPV4);

		/**
		 * Send data through the socket. This is the classic c-style signature version.
		 * 
		 * @param const void* buf  The data to be sent
		 * @param size_t 	  size The size of the buffer
		 * 
		 * @throw suc_error
		 */
		void send(const void* buf, size_t size);

		/**
		 * Send data through the socket.
		 * 
		 * @param const std::vector<sbyte>& buf
		 * 
		 * @throw suc_error
		 */
		void send(const std::vector<sbyte>& buf);

		/**
		 * Send data through the socket.
		 * 
		 * @param const std::string& str
		 * 
		 * @throw suc_error
		 */
		void send(const std::string& str);

		/**
		 * Read data from the socket.

		 * @param int timeout: Specifies the time in milliseconds that the socket will wait for incoming data.
		 * If the timeout parameter is set to 0, the method will return immediately if no data is available.
		 * If the timeout parameter is set to -1, the method will block until data is available.

		 * @return std::vector<sbyte> The received data. Is empty if the timeout has expired and no data has been
		 * received.
		 * 
		 * @throw suc_error
		 */
		[[nodiscard]]
		auto recv(int timeout = TIMEOUT_NEVER) -> std::vector<sbyte>;

		/**
		 * Read data from the socket.

		 * @param int timeoutMS: Specifies the time in milliseconds that the socket will wait for incoming data.
		 * If the timeout parameter is set to 0, the method will return immediately if no data is available.
		 * If the timeout parameter is set to -1, the method will block until data is available.

		 * @return std::string The received data as a string. Is empty if the timeout has expired and no data has been
		 * received.
		 * 
		 * @throw suc_error
		 */
		[[nodiscard]]
		auto recvString(int timeout = TIMEOUT_NEVER) -> std::string;

		/**
		 * Tests if the socket has data ready to read.
		 * 
		 * @param int timeout Specifies the time in milliseconds that the method waits for data to become available.
		 * If the timeout parameter is set to 0, the method will return immediately.
		 * If the timeout parameter is set to -1, the method will block until data is available.
		 * 
		 * @return bool True if data is available, false otherwise.
		 * Is also true if the connection has been closed remotely. If this is the case, the next call to recv()
		 * or recvString() returns nothing.
		 * 
		 * @throw suc_error
		 */
		[[nodiscard]]
		bool hasData(int timeout = TIMEOUT_NEVER) const;

		/**
		 * Close the socket.
		 * 
		 * @throw suc_error
		 */
		void close();

		/**
		 * Queries whether the socket has been closed.
		 * 
		 * @return bool True if the socket is closed, false othwerwise
		 */
		[[nodiscard]]
		bool isClosed() const noexcept;

	private:
		static constexpr size_t STANDARD_BUF_SIZE = 4096;

		SOCKET socket{ INVALID_SOCKET };
		bool _isClosed{ true };
	};
} // namespace suc



#endif
