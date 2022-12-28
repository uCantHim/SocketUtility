#include "client_socket.h"

#include <cassert>
#include <iostream>

#include "os.h"



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
    catch(const Error& e)
    {
        std::cerr << "In ClientSocket::~ClientSocket(): " << e.what() << '\n';
    }
}


suc::ClientSocket::ClientSocket(ClientSocket&& other) noexcept
{
    std::swap(socket, other.socket);
    std::swap(_isClosed, other._isClosed);
}


suc::ClientSocket& suc::ClientSocket::operator=(ClientSocket&& rhs) noexcept
{
    std::swap(socket, rhs.socket);
    std::swap(_isClosed, rhs._isClosed);

    return *this;
}


bool suc::ClientSocket::connect(const std::string& ip, int port, AddressType family)
{
    if (!_isClosed) { close(); }

    // Create a new socket
    addrinfo* ptr = translateAddress(ip, port, static_cast<int>(family),
                                     SOCK_STREAM, IPPROTO_TCP, 0);
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


void suc::ClientSocket::send(const void* buf, size_t size) const
{
    int writtenBytes = suc_send(socket, buf, size, 0);
    if (writtenBytes < 0) {
        handleLastError();
    }

    assert(writtenBytes == static_cast<int>(size));
}


auto suc::ClientSocket::recv(size_t maxBytes, Timeout timeout) const -> std::vector<std::byte>
{
    // Wait for the timeout
    if (!hasData(timeout)) {
        return {};
    }

    std::vector<std::byte> buf(maxBytes);
    int readBytes = suc_recv(socket, buf.data(), buf.size(), 0);
    if (readBytes == 0) {
        handleLastError();
    }

    // Truncate buffer size to the actually received number of bytes,
    // such that buf.size() == readBytes
    buf.resize(static_cast<size_t>(readBytes));

    return buf;
}

auto suc::ClientSocket::recvString(size_t maxBytes, Timeout timeout) const -> std::string
{
    auto result = recv(maxBytes, timeout);
    return std::string{ reinterpret_cast<char*>(result.data()), result.size() };
}

bool suc::ClientSocket::hasData(Timeout timeout) const
{
    // select() is POSIX-standardized but I still wrote a separate implementation
    // for it. I shall look into this.
    fd_set read{};
    FD_SET(socket, &read);

    constexpr std::int32_t secondsFactor = 1000L;
    timeval time{};
    time.tv_usec = static_cast<int32_t>(timeout.timeoutMs) % secondsFactor;
    time.tv_sec = (static_cast<int32_t>(timeout.timeoutMs) - time.tv_usec) / secondsFactor;

    timeval* t_ptr = &time;
    if (timeout == Timeout::never())
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


void suc::ClientSocket::send(const std::vector<std::byte>& buf) const
{
    send(buf.data(), buf.size());
}


void suc::ClientSocket::send(const std::string& str) const
{
    send(str.c_str(), str.size());
}


bool suc::ClientSocket::isClosed() const noexcept
{
    return _isClosed;
}
