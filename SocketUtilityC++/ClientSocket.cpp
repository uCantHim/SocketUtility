#include "ClientSocket.h"

#include "SucInternals.h"



suc::ClientSocket::ClientSocket(SOCKET socket) noexcept
	:
	socket(socket)
{
}


suc::ClientSocket::~ClientSocket()
{
	delete address;
	close();
}


bool suc::ClientSocket::connect(std::string ip, int port, int family)
{
	if (!_isClosed) { close(); }

	if (ip.empty()) {
		ip = SUC_ADDR_LOCAL_HOST;
	}

	addrinfo* ptr = sucTranslateAddress(ip, port, family, SOCK_STREAM, IPPROTO_TCP, NULL);
	int iResult = 0;

	for (; ptr != nullptr; ptr = ptr->ai_next)
	{
		// Create socket
		socket = winsock_socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (socket == INVALID_SOCKET)
		{
			// Do nothing, just try the next address
			continue;
		}

		iResult = winsock_connect(socket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
		if (iResult != SOCKET_ERROR)
		{
			_isClosed = false;
			return true;
		}

		sucHandleErrorCode(WSAGetLastError());
	}
	// No valid address was generated by sucTranslateAddress().
	sucHandleErrorCode(WSAGetLastError());
	return false;
}


void suc::ClientSocket::send(const sbyte* buf, uint size)
{
	int bytesSent = winsock_send(socket, buf, static_cast<int>(size), NULL);
	if (bytesSent < 0) {
		sucHandleErrorCode(WSAGetLastError());
	}
	assert(bytesSent == static_cast<int>(size));
}


void suc::ClientSocket::send(const ByteBuffer& buf)
{
	send(static_cast<sbyte*>(buf), static_cast<uint>(buf.size()));
}


void suc::ClientSocket::sendString(const std::string& to_str)
{
	send(static_cast<const sbyte*>(to_str.c_str()), static_cast<uint>(to_str.size()));
}


suc::uint suc::ClientSocket::recv(sbyte* buf, int timeoutMS)
{
	if (!hasData(timeoutMS)) {
		return 0U;
	}

	int bytes = winsock_recv(socket, static_cast<char*>(recvbuf), recvbuf.size(), NULL);
	if (bytes > recvbuf.size() - REMAINING_BUF_SPACE_CAP)
	{
		recvbuf.resize(recvbuf.size() * 2);
	}

	if (bytes > 0) // Received data
	{
		memcpy(buf, static_cast<void*> (recvbuf), static_cast<size_t>(bytes));
		return static_cast<uint>(bytes);
	}
	if (bytes == 0) // Connection has been closed remotely
	{
		throw SucSocketException("Connection closed remotely.");
	}
	
	sucHandleErrorCode(WSAGetLastError()); // bytes == SOCKET_ERROR
}


std::optional<suc::ByteBuffer> suc::ClientSocket::recv(int timeoutMS)
{
	uint bytes = recv(static_cast<sbyte*>(recvbuf), timeoutMS);
	if (bytes == 0) {
		return std::nullopt;
	}
	return ByteBuffer(bytes, static_cast<void*>(recvbuf));
}


std::string suc::ClientSocket::recvString(int timeoutMS)
{
	uint bytes = recv(static_cast<sbyte*>(recvbuf), timeoutMS);
	if (bytes == 0) {
		return std::string();
	}
	return std::string{ static_cast<char*>(recvbuf), bytes };
}


bool suc::ClientSocket::hasData(int timeoutMS) const
{
	constexpr std::int32_t secondsFactor = 1000L;
	timeval timeout;
	timeout.tv_usec = static_cast<std::int32_t>(timeoutMS) % secondsFactor;
	timeout.tv_sec = (static_cast<std::int32_t>(timeoutMS) - timeout.tv_usec) / secondsFactor;

	fd_set read{ 1, {socket} };
	int numSockets = winsock_select(NULL, &read, nullptr, nullptr, timeoutMS != -1 ? &timeout : nullptr);

	if (numSockets == 1) {
		return true;
	}
	if (numSockets == 0) { // Timeout
		return false;
	}
	if (numSockets == SOCKET_ERROR) {
		sucHandleErrorCode(WSAGetLastError());
	}
}


void suc::ClientSocket::close()
{
	if (_isClosed) { return; }

	int iResult = closesocket(socket);
	if (iResult == SOCKET_ERROR) {
		sucHandleErrorCode(WSAGetLastError());
	}

	_isClosed = true;
}

bool suc::ClientSocket::isClosed() const noexcept
{
	return _isClosed;
}