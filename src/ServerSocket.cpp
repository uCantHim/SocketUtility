#include "ServerSocket.h"

#include <cassert>
#include <iostream>
#include <string>

#include "ClientSocket.h"
#include "SucInternals.h"



suc::ServerSocket::ServerSocket(int port, int family)
{
	bind(port, family);
}


suc::ServerSocket::~ServerSocket() noexcept
{
	try
	{
		close();
	}
	catch(const suc_error& e)
	{
		std::cerr << "In ServerSocket::~ServerSocket(): " << e.what() << '\n';
	}
}


suc::ServerSocket::ServerSocket(ServerSocket&& other) noexcept
{
	std::swap(_isClosed, other._isClosed);
	std::swap(socket, other.socket);
	std::swap(address, other.address);
}


suc::ServerSocket& suc::ServerSocket::operator=(ServerSocket&& rhs) noexcept
{
	std::swap(_isClosed, rhs._isClosed);
	std::swap(socket, rhs.socket);
	std::swap(address, rhs.address);

	return *this;
}


void suc::ServerSocket::bind(int port, int family)
{
	if (!(family == IPV4 || family == IPV6)) {
		throw value_error("Invalid family: " + std::to_string(family));
	}

	// Create a new socket
	socket = suc_socket(family, SOCK_STREAM, 0);
	if (socket == -1)
		handleLastError();

	// Bind to localhost
	memset(&address, 0, sizeof(address));
	address.sin_family = family;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;

	int result = suc_bind(socket, reinterpret_cast<sockaddr*>(&address), sizeof(address));
	if (result == -1)
		handleLastError();

	// Listen
	const int backlogQueueSize = 5; // Maximum on most systems
	suc_listen(socket, backlogQueueSize); // Cannot fail if the first argument is a valid socket descriptor

	_isClosed = false;
}


auto suc::ServerSocket::accept() const -> ClientSocket
{
	sockaddr_in clientAddress{};
	int addressLength = static_cast<int>(sizeof(clientAddress));
	SOCKET newSock = suc_accept(
		socket,
		reinterpret_cast<sockaddr*>(&clientAddress),
		&addressLength
	);

	if (newSock == -1)
		handleLastError();

	// Create ClientSocket
	return ClientSocket(newSock);
}


void suc::ServerSocket::close()
{
	if (_isClosed) { return; }

	if (suc_close(socket))
		handleLastError();

	_isClosed = true;
}


bool suc::ServerSocket::isClosed() const noexcept
{
	return _isClosed;
}
