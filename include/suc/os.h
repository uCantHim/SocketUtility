#pragma once

// ----------------------------------------------------- //
//                                                       //
//        Cross-platform implementation internals        //
//                                                       //
// ----------------------------------------------------- //

// Operating system macros
#ifdef _WIN32
    #define OS_IS_WINDOWS
#endif
#ifdef __linux__
    #define OS_IS_LINUX
#endif

// Include windows socket headers
#ifdef OS_IS_WINDOWS
    #include <WinSock2.h>
    #include <WS2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#endif

// Include Linux socket headers
#ifdef OS_IS_LINUX
    #include <arpa/inet.h> // Might fix some segfault
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/select.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <unistd.h> // write() and read()

    #define SOCKET int
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
#endif

#include <string>

#ifdef OS_IS_WINDOWS

constexpr auto SUC_WSA_VERSION_MAJOR = 2;
constexpr auto SUC_WSA_VERSION_MINOR = 2;

// -------------- //
// Initialize WSA //
// -------------- //
#ifndef WSA_INITIALIZED
#define WSA_INITIALIZED

namespace wsa_init
{
    // Initialize WSA because Microsoft is retarded
    static bool sucWsaInitResult = []() {
        WSADATA wsaData;

        int iResult = WSAStartup(MAKEWORD(SUC_WSA_VERSION_MAJOR, SUC_WSA_VERSION_MINOR), &wsaData);
        if (iResult != 0)
        {
            WSACleanup();
            handleLastError();
        }

        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
        {
            WSACleanup();
            throw suc::system_error("WSA_INIT_ERROR: Could not find a usable Winsock 2.2 dll.\n");
        }

        return true;
    }();
} // namespace wsa_init

#endif // #ifndef WSA_INITIALIZED
#endif // #ifdef OS_IS_WINDOWS

SOCKET suc_socket  (int domain, int type, int protocol);
int    suc_bind    (SOCKET s, sockaddr* addr, int addrlen);
int    suc_listen  (SOCKET s, int backlog);
SOCKET suc_accept  (SOCKET s, sockaddr* addr, int* addrlen);
int    suc_connect (SOCKET s, sockaddr* addr, int addrlen);
int    suc_recv    (SOCKET s, void* buf, size_t len, int flags);
int    suc_send    (SOCKET s, const void* buf, size_t len, int flags);
int    suc_select  (int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, timeval* timeout);
int    suc_close   (SOCKET s);

/**
 * @brief Resolve an IP address to an addrinfo struct
 *
 * @param ip_address: The IP address in readable string format.
 * @param port: The port.
 * @param family: Either SUC_Family::eIPv4, SUC_Family::eIPv6 or SUC_Family::eIPvX
 * @param type: SOCK_STREAM
 * @param protocol: IPPROTO_TCP
 * @param flags: NULL
 *
 * @return Returns a addrinfo structure if the translation has been
 *         successful, nullptr otherwise.
 */
auto translateAddress(const std::string& ip_address,
                      int port,
                      int family,
                      int type,
                      int protocol,
                      int flags) -> addrinfo*;

inline auto getLastError()
{
#ifdef OS_IS_WINDOWS
    return WSAGetLastError();
#endif
#ifdef OS_IS_LINUX
    return errno;
#endif
}

/**
 * @brief Generate a suitable exception for the lastest error
 */
[[noreturn]]
extern void handleLastError();

#ifdef OS_IS_WINDOWS
    #define EACCES WSAEACCES
    #define EADDRINUSE WSAEADDRINUSE
    #define EBADF WSAEBADF
    #define EINVAL WSAEINVAL
    #define ENOTSOCK WASENOTSOCK
    #define EADDRNOTAVAIL WSAEADDRNOTAVAIL
    #define EFAULT WSAEFAULT
    #define ELOOP WSAELOOP
    #define ENAMETOOLONG WSAENAMETOOLONG
    #define ENOENT WSAENOENT
    #define ENOMEM WSAENOMEM
    #define ENOTDIR WSAENOTDIR
    #define EROFS WSAEROFS
    #define EAFNOSUPPORT WSAEAFNOSUPPORT
    #define EALREADY WSAEALREADY
    #define ECONNREFUSED WSAECONNREFUSED
    #define EINPROGRESS WSAEINPROGRESS
    #define EINTR WSAEINTR
    #define EISCONN WSAEISCONN
    #define ENETUNREACH WSAENETUNREACH
    #define ETIMEDOUT WSAETIMEDOUT
    #define EIO WSAEIO
    #define EPROTOTYPE WSAEPROTOTYPE
#endif
