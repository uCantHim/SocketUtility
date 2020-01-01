#pragma once
#ifndef SUCINTERNALS_H
#define SUCINTERNALS_H

#include "SocketUtility.h"

constexpr auto ADDRESS_TRANSLATE_MAX_TRY_AGAIN = 10;

/* +++ handleLastError() +++
Generates a suitable exception for the lastest error. */
[[noreturn]] static void handleLastError();

/* +++ translateAddress() +++
Translates an IP address in string representation (xxx.xxx.xxx.xxx) into a WSA-addrinfo.

- ARG ip_address: The IP address in readable string format.
- ARG port: The port.
- ARG family: Either SUC_IPV4, SUC_IPV6 or SUC_IPVX
- ARG type: SOCK_STREAM
- ARG protocol: IPPROTO_TCP
- ARG flags: NULL

- RETURN: Returns a addrinfo-structure if the translation has been successful, nullptr otherwise.
*/
static addrinfo* translateAddress(
	const std::string& ip_address, int port,
	int family, int type, int protocol, int flags
);


#ifdef OS_IS_WINDOWS
// Initialize WSA because Microsoft is retarded

constexpr auto SUC_WSA_VERSION_MAJOR = 2;
constexpr auto SUC_WSA_VERSION_MINOR = 2;

// -------------- //
// Initialize WSA //
// -------------- //
#ifndef WSA_INITIALIZED
#define WSA_INITIALIZED

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


#endif // #ifndef WSA_INITIALIZED
#endif // #ifdef OS_IS_WINDOWS


// ------------------------------------------------ //
//                                                  //
//		Cross-platform implementation internals		//
//                                                  //
// ------------------------------------------------ //


// SOCKET
static SOCKET suc_socket(int domain, int type, int protocol)
{
	return socket(domain, type, protocol);
}

// BIND
static int suc_bind(SOCKET s, sockaddr* addr, int addrlen)
{
#ifdef OS_IS_WINDOWS
	return bind(s, addr, addrlen);
#endif
#ifdef OS_IS_LINUX
	return bind(s, addr, static_cast<socklen_t>(addrlen));
#endif
}

// LISTEN
static int suc_listen(SOCKET s, int backlog)
{
	return listen(s, backlog);
}

// ACCEPT
static SOCKET suc_accept(SOCKET s, sockaddr* addr, int* addrlen)
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
static int suc_connect(SOCKET s, sockaddr* addr, int addrlen)
{
#ifdef OS_IS_WINDOWS
	return connect(s, addr, addrlen);
#endif
#ifdef OS_IS_LINUX
	return connect(s, addr, static_cast<socklen_t>(addrlen));
#endif
}

// RECV
static int suc_recv(SOCKET s, void* buf, size_t len, int flags)
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
static int suc_send(SOCKET s, const void* buf, size_t len, int flags)
{
#ifdef OS_IS_WINDOWS
	return send(s, reinterpret_cast<const char*>(buf), static_cast<int>(len), flags);
#endif
#ifdef OS_IS_LINUX
	return static_cast<int>(send(s, buf, len, flags));
#endif
}

// SELECT
static int suc_select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, timeval* timeout)
{
	return select(nfds, readfds, writefds, exceptfds, timeout);
}

// CLOSE
static int suc_close(SOCKET s)
{
#ifdef OS_IS_WINDOWS
	return closesocket(s);
#endif
#ifdef OS_IS_LINUX
	return close(s);
#endif
}


static addrinfo* translateAddress(
	const std::string& ip_address, int port,
	int family, int type, int protocol, int flags)
{
	std::string portStr = std::to_string(port);
	addrinfo hints = { 0 };
	addrinfo* result = nullptr;
	int iResult = 0;

	hints.ai_family = family;
	hints.ai_socktype = type;
	hints.ai_protocol = protocol;
	hints.ai_flags = flags;

	// Translate address
	iResult = getaddrinfo(nullptr, portStr.c_str(), &hints, &result);

	// iResult == WSATRY_AGAIN
	for (int attempts = 0; iResult == EAI_AGAIN && attempts < ADDRESS_TRANSLATE_MAX_TRY_AGAIN; attempts++) {
		iResult = getaddrinfo(ip_address.c_str(), portStr.c_str(), &hints, &result);
	}
	if (iResult != 0) {
		handleLastError();
	}

	return result;
}


static auto getLastError()
{
#ifdef OS_IS_WINDOWS
	return WSAGetLastError();
#endif
#ifdef OS_IS_LINUX
	return errno;
#endif
}


[[noreturn]] static void handleLastError()
{
	auto lastError = getLastError();

	switch(lastError)
	{
	case EACCES:
		throw suc::suc_error("Access error.");	
	case EADDRINUSE:
		throw suc::suc_error("Address in use.");
	case EBADF:
		throw suc::suc_error("Socket is not a valid descriptor.");
	case EINVAL:
		throw suc::suc_error(
			"Socket is already bound to an address or the specified address length is not a valid length for an address"
			" of the specified address family.");
	case ENOTSOCK:
		throw suc::suc_error("Socket is not a socket descriptor.");
	case EADDRNOTAVAIL:
		throw suc::suc_error("Address not available.");
	case EFAULT:
		throw suc::suc_error("Bad memory access.");
	case ELOOP:
		throw suc::suc_error("Too many symbolc links.");
	case ENAMETOOLONG:
		throw suc::suc_error("Name too long.");
	case ENOENT:
		throw suc::suc_error("File does not exist.");
	case ENOMEM:
		throw suc::suc_error("Out of memory.");
	case ENOTDIR:
		throw suc::suc_error("Not a directory.");
	case EROFS:
		throw suc::suc_error("Read only.");
	case EAFNOSUPPORT:
		throw suc::suc_error("Address not supported in this family.");
	case EALREADY:
		throw suc::suc_error("Socket is non-blocking and a previous connection attempt has not yet benn completed.");
	case ECONNREFUSED:
		throw suc::suc_error("Connection refused.");
	case EINPROGRESS:
		throw suc::suc_error("The socket is non-blocking and the connection cannot be completed immediately.");
	case EINTR:
		throw suc::suc_error("The connection attempt was interrupted before any data arrived by the delivery of a signal.");
	case EISCONN:
		throw suc::suc_error("The socket is already connected.");
	case ENETUNREACH:
		throw suc::suc_error("The network is not reachable from this host.");
	case ETIMEDOUT:
		throw suc::suc_error("Connection timeout.");
	case EIO:
		throw suc::suc_error("An I/O error occured during writing to or reading from the file system.");
	case EPROTOTYPE:
		throw suc::suc_error("The socket referred to by name is of another type than s.");
	//case E:
		//throw suc::suc_error("");

	default:
		throw suc::suc_error("Unknown error code: " + std::to_string(lastError));
	}
}

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



#endif // Header guard
