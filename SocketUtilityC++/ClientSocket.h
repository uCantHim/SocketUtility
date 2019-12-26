#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <string>
#include <optional>

#include "SocketUtility.h"

namespace suc
{
	class ClientSocket
	{
	public:
		ClientSocket() = default;
		explicit ClientSocket(SOCKET socket) noexcept;

		ClientSocket(const ClientSocket&) = delete;
		ClientSocket(ClientSocket&&) noexcept = default;
		~ClientSocket() noexcept;

		ClientSocket& operator=(const ClientSocket&) = delete;
		ClientSocket& operator=(ClientSocket&&) noexcept = default;

		/* +++ connect() +++
		Attempts to connect to a server.
		- ARG ip: Specifies the server's IP address. If this is an empty string, the socket will
		instead use the local host address.
		- ARG port: Specifies the server's port.
		- ARG family: Must be either SUC_IPV4, SUC_IPV6 or SUC_IPVX.
		- RETURN: Returns true if a connection could be established, false otherwise. */
		bool connect(std::string ip, int port, int family = SUC_IPV4);

		/* +++ sned() +++
		Sends data through the socket.
		- ARG buf: Specifies the data that shall be sent through the socket.
		- ARG size: Specifies the size of the data buffer (the number of bytes to be sent). */
		void send(const sbyte* buf, uint size);

		/* +++ sned() +++
		Sends data through the socket.
		- ARG buf: Specifies the data that shall be sent through the socket. */
		void send(const std::vector<sbyte>& buf);

		/* +++ sendString() +++
		Sends a string through the socket.
		ARG str: The string that will be sent to the client. */
		void sendString(const std::string& to_str);

		/* +++ recv() +++
		Attempts to read data from the socket.
		- ARG buf: The buffer the data will be read to.
		- ARG timeoutMS: Specifies the time in milliseconds that the socket will wait for incoming data.
		If the timeout parameter is set to 0, the method will return immediately if no data is available.
		If the timeout parameter is set to -1, the method will block until data is available.
		- RETURN: Returns the number of bytes received. */
		[[nodiscard]] uint recv(sbyte* buf, int timeoutMS = SUC_TIMEOUT_NEVER);

		/* +++ recv() +++
		Attempts to read data from the socket.
		- ARG timeoutMS: Specifies the time in milliseconds that the socket will wait for incoming data.
		If the timeout parameter is set to 0, the method will return immediately if no data is available.
		If the timeout parameter is set to -1, the method will block until data is available.
		- RETURN: Returns a buffer containing the received data if any data has been received. */
		[[nodiscard]] std::optional<std::vector<sbyte>> recv(int timeoutMS = SUC_TIMEOUT_NEVER);

		/* +++ recvString() +++
		Attempts to read data from the socket.
		- ARG timeoutMS: Specifies the time in milliseconds that the socket will wait for incoming data.
		If the timeout parameter is set to 0, the method will return immediately if no data is available.
		If the timeout parameter is set to -1, the method will block until data is available.
		RETURN: Returns the received data in string form. If no data could be read, this returns an empty string. */
		[[nodiscard]] std::string recvString(int timeoutMS = SUC_TIMEOUT_NEVER);

		/* +++ hasData() +++
		Checks whether the socket has data available to be read.
		- ARG timeoutMS: Specifies the time in milliseconds that the method shall for data to be available.
		If the timeout parameter is set to 0, the method will return immediately.
		If the timeout parameter is set to -1, the method will block until data is available.
		- RETURN: Returns true if data is available, false otherwise.
		The return value true may also indicate that the connection has been closed. This is the case if
		the next call to recv() or recvString() returns zero bytes. */
		[[nodiscard]] bool hasData(int timeoutMS = 0) const;

		/* +++ close() +++
		Closes the socket. */
		void close();

		/* +++ isClosed() +++
		- RETURN: Returns true if the socket has been closed. */
		[[nodiscard]] bool isClosed() const noexcept;

	private:
		static constexpr uint STANDARD_BUF_SIZE = 4096;
		static constexpr int REMAINING_BUF_SPACE_CAP = 20;

		std::vector<sbyte> recvbuf = std::vector<sbyte>(STANDARD_BUF_SIZE);

		SOCKET socket{ INVALID_SOCKET };
		bool _isClosed{ true };
	};
} // namespace suc



#endif
