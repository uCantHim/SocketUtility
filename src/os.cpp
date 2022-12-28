#include "os.h"

#include "util.h"



SOCKET suc_socket(int domain, int type, int protocol)
{
    return socket(domain, type, protocol);
}

// BIND
int suc_bind(SOCKET s, sockaddr* addr, int addrlen)
{
#ifdef OS_IS_WINDOWS
    return bind(s, addr, addrlen);
#endif
#ifdef OS_IS_LINUX
    return bind(s, addr, static_cast<socklen_t>(addrlen));
#endif
}

// LISTEN
int suc_listen(SOCKET s, int backlog)
{
    return listen(s, backlog);
}

// ACCEPT
SOCKET suc_accept(SOCKET s, sockaddr* addr, int* addrlen)
{
#ifdef OS_IS_WINDOWS
    return accept(s, addr, static_cast<socklen_t*>(addrlen));
#endif
#ifdef OS_IS_LINUX
    // Use socklen_t because Linux is retarded
    auto _addrlen = static_cast<socklen_t>(*addrlen);
    return accept(s, addr, &_addrlen);
#endif
}

// CONNECT
int suc_connect(SOCKET s, sockaddr* addr, int addrlen)
{
#ifdef OS_IS_WINDOWS
    return connect(s, addr, addrlen);
#endif
#ifdef OS_IS_LINUX
    return connect(s, addr, static_cast<socklen_t>(addrlen));
#endif
}

// RECV
int suc_recv(SOCKET s, void* buf, size_t len, int flags)
{
#ifdef OS_IS_WINDOWS
    // Cast buffer to char* because Microsoft is retarded
    return recv(s, reinterpret_cast<char*>(buf), static_cast<int>(len), flags);
#endif
#ifdef OS_IS_LINUX
    return static_cast<int>(recv(s, buf, len, flags));
#endif
}

// SEND
int suc_send(SOCKET s, const void* buf, size_t len, int flags)
{
#ifdef OS_IS_WINDOWS
    return send(s, reinterpret_cast<const char*>(buf), static_cast<int>(len), flags);
#endif
#ifdef OS_IS_LINUX
    return static_cast<int>(send(s, buf, len, flags));
#endif
}

// SELECT
int suc_select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, timeval* timeout)
{
    return select(nfds, readfds, writefds, exceptfds, timeout);
}

// CLOSE
int suc_close(SOCKET s)
{
#ifdef OS_IS_WINDOWS
    return closesocket(s);
#endif
#ifdef OS_IS_LINUX
    return close(s);
#endif
}

auto translateAddress(
    const std::string& ip_address,
    int port,
    int family,
    int type,
    int protocol,
    int flags) -> addrinfo*
{
    constexpr auto kAddrTranslateMaxRetries = 10;

    std::string portStr = std::to_string(port);
    addrinfo hints{};
    addrinfo* result{ nullptr };
    int iResult{ 0 };

    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;
    hints.ai_flags = flags;

    // Translate address
    iResult = getaddrinfo(nullptr, portStr.c_str(), &hints, &result);

    // iResult == WSATRY_AGAIN
    for (int attempts = 0; iResult == EAI_AGAIN && attempts < kAddrTranslateMaxRetries; attempts++) {
        iResult = getaddrinfo(ip_address.c_str(), portStr.c_str(), &hints, &result);
    }
    if (iResult != 0) {
        handleLastError();
    }

    return result;
}

[[noreturn]]
void handleLastError()
{
    const auto lastError = getLastError();
    switch(lastError)
    {
    case EACCES:
        throw suc::Error("Access error.");
    case EADDRINUSE:
        throw suc::Error("Address in use.");
    case EBADF:
        throw suc::Error("Socket is not a valid descriptor.");
    case EINVAL:
        throw suc::Error(
            "Socket is already bound to an address or the specified address length is not a valid"
            " length for an address of the specified address family.");
    case ENOTSOCK:
        throw suc::Error("Socket is not a socket descriptor.");
    case EADDRNOTAVAIL:
        throw suc::Error("Address not available.");
    case EFAULT:
        throw suc::Error("Bad memory access.");
    case ELOOP:
        throw suc::Error("Too many symbolc links.");
    case ENAMETOOLONG:
        throw suc::Error("Name too long.");
    case ENOENT:
        throw suc::Error("File does not exist.");
    case ENOMEM:
        throw suc::Error("Out of memory.");
    case ENOTDIR:
        throw suc::Error("Not a directory.");
    case EROFS:
        throw suc::Error("Read only.");
    case EAFNOSUPPORT:
        throw suc::Error("Address not supported in this family.");
    case EALREADY:
        throw suc::Error("Socket is non-blocking and a previous connection attempt has not yet benn completed.");
    case ECONNREFUSED:
        throw suc::Error("Connection refused.");
    case EINPROGRESS:
        throw suc::Error("The socket is non-blocking and the connection cannot be completed immediately.");
    case EINTR:
        throw suc::Error("The connection attempt was interrupted before any data arrived by the delivery of a signal.");
    case EISCONN:
        throw suc::Error("The socket is already connected.");
    case ENETUNREACH:
        throw suc::Error("The network is not reachable from this host.");
    case ETIMEDOUT:
        throw suc::Error("Connection timeout.");
    case EIO:
        throw suc::Error("An I/O error occured during writing to or reading from the file system.");
    case EPROTOTYPE:
        throw suc::Error("The socket referred to by name is of another type than s.");
    //case E:
        //throw suc::suc_error("");

    default:
        throw suc::Error("Unknown error code: " + std::to_string(lastError));
    }
}
