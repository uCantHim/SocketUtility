#pragma once
#ifndef SUCINTERNALS_H
#define SUCINTERNALS_H

#include "SocketUtility.h"

// -------------------------------------------- //
//		Windows implementation internals		//
// -------------------------------------------- //

/* +++ handleLastError() +++
Generates a suitable exception for the lastest error. */
[[noreturn]] static void handleLastError();


#ifdef OS_IS_WINDOWS

constexpr auto SUC_WSA_VERSION_MAJOR = 2;
constexpr auto SUC_WSA_VERSION_MINOR = 2;
constexpr auto SUC_ADDRESS_TRANSLATE_MAX_TRY_AGAIN = 10;


// -------------- //
// Function heads //
// -------------- //

/* +++ sucTranslateAddress() +++
Translates an IP address in string representation (xxx.xxx.xxx.xxx) into a WSA-addrinfo.

- ARG ip_address: The IP address in readable string format.
- ARG port: The port.
- ARG family: Either SUC_IPV4, SUC_IPV6 or SUC_IPVX
- ARG type: SOCK_STREAM
- ARG protocol: IPPROTO_TCP
- ARG flags: NULL

- RETURN: Returns a addrinfo-structure if the translation has been successful, nullptr otherwise.
*/
static addrinfo* sucTranslateAddress(
	const std::string& ip_address, int port,
	int family, int type, int protocol, int flags
);


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



// --------------------------------------------------------------------	//
//						WinSock-functions rename						//
//																		//
// I have to do this to avoid naming conflicts with Socket methods.		//
// -------------------------------------------------------------------- //

static SOCKET winsock_socket(int af, int type, int protocol)
{
	return socket(af, type, protocol);
}
static int winsock_bind(SOCKET s, const sockaddr* name, int namelen)
{
	return bind(s, name, namelen);
}
static int winsock_listen(SOCKET s, int backlog)
{
	return listen(s, backlog);
}
static SOCKET winsock_accept(SOCKET s, sockaddr* addr, int* addrlen)
{
	return accept(s, addr, addrlen);
}
static int winsock_connect(SOCKET s, const sockaddr* name, int namelen)
{
	return connect(s, name, namelen);
}
static int winsock_recv(SOCKET s, char* buf, int len, int flags)
{
	return recv(s, buf, len, flags);
}
static int winsock_send(SOCKET s, const char* buf, int len, int flags)
{
	return send(s, buf, len, flags);
}
static int winsock_select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const timeval* timeout)
{
	return select(nfds, readfds, writefds, exceptfds, timeout);
}



// ----------------------------------------------------	//
//					Helper functions					//
//														//
// These help me with some recurring WinSock tasks.		//
// ---------------------------------------------------- //

static addrinfo* sucTranslateAddress(
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
	for (int attempts = 0; iResult == WSATRY_AGAIN && attempts < SUC_ADDRESS_TRANSLATE_MAX_TRY_AGAIN; attempts++) {
		iResult = getaddrinfo(ip_address.c_str(), portStr.c_str(), &hints, &result);
	}
	if (iResult != 0) {
		handleLastError();
	}

	return result;
}

#endif // #ifdef OS_IS_WINDOWS



// ---------------------------------------- //
//		Linux implementation internals		//
// ---------------------------------------- //

#ifdef OS_IS_LINUX

static SOCKET linux_socket(int domain, int type, int protocol)
{
	return socket(domain, type, protocol);
}

static int linux_bind(int s, sockaddr* name, int namelen)
{
	return bind(s, name, namelen);
}

static int linux_listen(int s, int backlog)
{
	return listen(s, backlog);
}

static SOCKET linux_accept(int s, sockaddr* addr, socklen_t* addrlen)
{
	return accept(s, addr, addrlen);
}

static int linux_connect(int s, sockaddr* name, int namelen)
{
	return connect(s, name, namelen);
}

static int linux_close(int fd)
{
	return close(fd);
}

#endif // #ifdef OS_IS_LINUX



[[noreturn]] static void handleLastError()
{
#ifdef OS_IS_WINDOWS
	auto lastError = WSAGetLastError();
#endif
#ifdef OS_IS_LINUX
	auto lastError = errno;
#endif

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

//#ifdef OS_IS_WINDOWS
//	#define EACCES WSAEACCES
//	#define EADDRINUSE WSAEADDRINUSE
//	#define EBADF WSAEBADF
//	#define EINVAL WSAEINVAL
//	#define ENOTSOCK WASENOTSOCK
//	#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
//	#define EFAULT WSAEFAULT
//	#define ELOOP WSAELOOP
//	#define ENAMETOOLONG WSAENAMETOOLONG
//	#define ENOENT WSAENOENT
//	#define ENOMEM WSAENOMEM
//	#define ENOTDIR WSAENOTDIR
//	#define EROFS WSAEROFS
//	#define EAFNOSUPPORT WSAEAFNOSUPPORT
//	#define EALREADY WSAEALREADY
//	#define ECONNREFUSED WSAECONNREFUSED
//	#define EINPROGRESS WSAEINPROGRESS
//	#define EINTR WSAEINTR
//	#define EISCONN WSAEISCONN
//	#define ENETUNREACH WSAENETUNREACH
//	#define ETIMEDOUT WSAETIMEDOUT
//	#define EIO WSAEIO
//	#define EPROTOTYPE WSAEPROTOTYPE
//#endif



#endif // Header guard