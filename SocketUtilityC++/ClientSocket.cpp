#include "ClientSocket.h"

#include "SucInternals.h"



suc::ClientSocket::ClientSocket(SOCKET socket) noexcept
	:
	socket(socket)
{
}


suc::ClientSocket::~ClientSocket() noexcept
{
	try
	{
		close();
	}
	catch(const suc_error& e)
	{
		std::cerr << "In ClientSocket::~ClientSocket(): " << e.what() << '\n';
	}
}


suc::ClientSocket::ClientSocket(ClientSocket&& other) noexcept
{
	std::swap(socket, other.socket);
	std::swap(recvbuf, other.recvbuf);
	std::swap(_isClosed, other._isClosed);
}


suc::ClientSocket& suc::ClientSocket::operator=(ClientSocket&& rhs) noexcept
{
	std::swap(socket, rhs.socket);
	std::swap(recvbuf, rhs.recvbuf);
	std::swap(_isClosed, rhs._isClosed);

	return *this;
}


bool suc::ClientSocket::connect(std::string ip, int port, int family)
{
	if (!_isClosed) { close(); }
	if (ip.empty()) {
		ip = ADDR_LOCALHOST_4;
	}

	// Create a new socket
    addrinfo* ptr = translateAddress(ip, port, family, SOCK_STREAM, IPPROTO_TCP, 0);
	int iResult = 0;

	for (; ptr != nullptr; ptr = ptr->ai_next)
	{
		// Create socket
		socket = suc_socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (socket == INVALID_SOCKET)
		{
			// Do nothing, just try the next address
			continue;
		}

		iResult = suc_connect(socket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
		if (iResult != SOCKET_ERROR)
		{
			_isClosed = false;
			return true;
		}

		handleLastError();
	}
    
	// No returned addresses were valid
	handleLastError();
}


void suc::ClientSocket::send(const void* buf, size_t size)
{
	int writtenBytes = suc_send(socket, buf, size, 0);
	if (writtenBytes < 0)
		handleLastError();

	assert(writtenBytes == static_cast<int>(size));
}


suc::uint suc::ClientSocket::recv(void* buf, int timeoutMS)
{
	if (!hasData(timeoutMS)) {
		return 0U;
	}

	int readBytes = suc_recv(socket, recvbuf.data(), recvbuf.size(), 0);
	if (readBytes > recvbuf.size() - REMAINING_BUF_SPACE_CAP)
	{
		recvbuf.resize(recvbuf.size() * 2);
	}

	if (readBytes > 0) // Received data
	{
		memcpy(buf, recvbuf.data(), static_cast<size_t>(readBytes));
		return static_cast<uint>(readBytes);
	}
	if (readBytes == 0) // Connection has been closed remotely
	{
		handleLastError();
	}

	handleLastError();
}


bool suc::ClientSocket::hasData(int timeoutMS) const
{
	// select() is POSIX-standardized but I still wrote a separate implementation
	// for it. I shall look into this.
	fd_set read{};
	FD_SET(socket, &read);

	constexpr std::int32_t secondsFactor = 1000L;
	timeval timeout{};
	timeout.tv_usec = static_cast<std::int32_t>(timeoutMS) % secondsFactor;
	timeout.tv_sec = (static_cast<std::int32_t>(timeoutMS) - timeout.tv_usec) / secondsFactor;

	timeval* t_ptr = &timeout;
	if (timeoutMS == -1)
		t_ptr = nullptr;

	int numSockets = suc_select(socket + 1, &read, nullptr, nullptr, t_ptr);
	if (numSockets == -1)
		handleLastError();

	return numSockets > 0;
}


void suc::ClientSocket::close()
{
	if (_isClosed) { return; }

	// Don't throw if the descriptor is not a socket since that's the goal of this function anyway
	if (suc_close(socket) == -1 && getLastError() != ENOTSOCK)
		handleLastError();

	_isClosed = true;
}


void suc::ClientSocket::send(const std::vector<sbyte>& buf)
{
	send(buf.data(), buf.size());
}


void suc::ClientSocket::sendString(const std::string& str)
{
	send(str.c_str(), str.size());
}


std::optional<std::vector<suc::sbyte>> suc::ClientSocket::recv(int timeoutMS)
{
	uint bytes = recv(recvbuf.data(), timeoutMS);
	if (bytes == 0) {
		return std::nullopt;
	}

	return recvbuf;
}


std::string suc::ClientSocket::recvString(int timeoutMS)
{
	uint bytes = recv(recvbuf.data(), timeoutMS);
	if (bytes == 0) {
		return std::string();
	}

	return std::string{ recvbuf.data(), bytes };
}


bool suc::ClientSocket::isClosed() const noexcept
{
	return _isClosed;
}
