#pragma once
#ifndef SUCINTERNALS_H
#define SUCINTERNALS_H

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "SocketUtility.h"

constexpr auto SUC_WSA_VERSION_MAJOR = 2;
constexpr auto SUC_WSA_VERSION_MINOR = 2;
constexpr auto SUC_ADDRESS_TRANSLATE_MAX_TRY_AGAIN = 10;


// -------------- //
// Function heads //
// -------------- //

/* +++ sucHandleErrorCode() +++
Analyses the passed WSA-error-code and might generate an appropriate exception.
Usually, WSAGetLastError() is passed as the argument. */
[[noreturn]] static inline void sucHandleErrorCode(int err_code);

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

static bool sucInitWSA() {
	WSADATA wsaData;

	int iResult = WSAStartup(MAKEWORD(SUC_WSA_VERSION_MAJOR, SUC_WSA_VERSION_MINOR), &wsaData);
	if (iResult != 0)
	{
		WSACleanup();
		sucHandleErrorCode(iResult);
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		throw suc::SucSystemException("WSA_INIT_ERROR: Could not find a usable Winsock 2.2 dll.\n");
	}

	return true;
}

static bool sucWsaInitResult = sucInitWSA();

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

[[noreturn]] static inline void sucHandleErrorCode(int err_code)
{
	switch (err_code)
	{
	case WSAENETDOWN: // System
		throw suc::SucSystemException("The network subsystem has failed.");
		break;
	case WSAENOBUFS: // System
		throw suc::SucSystemException("No buffer space is available for the requested operation.");
		break;
	case WSAEADDRINUSE: // System
		throw suc::SucSystemException("The address is already in use.");
		break;
	case WSASYSNOTREADY: // System
		throw suc::SucSystemException("Underlying networking system not ready.\n");
		break;
	case WSANOTINITIALISED: // WSA
		throw suc::SucWSAException("WSA not initialized.");
		break;
	case WSAEACCES: // WSA
		throw suc::SucWSAException("An attempt was made to access a socket in a way forbidden by its access permissions.");
		break;
	case WSAEMFILE: // WSA
		throw suc::SucWSAException("Too many open sockets.");
		break;
	case WSANO_RECOVERY: // WSA
		throw suc::SucWSAException("A nonrecoverable failure occured during database lookup.");
		break;
	case WSAHOST_NOT_FOUND: // WSA
		throw suc::SucWSAException("Host not found.");
		break;
	case WSATYPE_NOT_FOUND: // WSA
		throw suc::SucWSAException("Class type not found.");
		break;
	case WSAESOCKTNOSUPPORT: // WSA
		throw suc::SucWSAException("The support for the specified socket type does not exist in this address family.");
		break;
	case WSAVERNOTSUPPORTED: // WSA
		throw suc::SucWSAException("WSA_INIT_ERROR: Requested version of Windows Sockets support not provided.\n");
		break;
	case WSAEPROCLIM: // WSA
		throw suc::SucWSAException("WSA_INIT_ERROR: A limit on the number of tasks supported by the Windows Sockets implementation has been reached.\n");
		break;
	case WSAEFAULT: // WSA
		throw suc::SucWSAException("An invalid pointer has been passed as an argument");
		break;
	case WSAEINPROGRESS: // WSA
		throw suc::SucWSAException("Another blocking operation is already in progress. WinSock allows only one blocking operation at a time.");
		break;
	case WSAEALREADY: // WSA
		throw suc::SucWSAException("A nonblocking operation is already in progress on the specified socket.");
		break;
	case WSAENOTSOCK: // WSA
		throw suc::SucWSAException("The descriptor is not a socket.");
		break;
	case WSAEOPNOTSUPP: // WSA
		throw suc::SucWSAException("Operation not supported for the type of object.");
		break;
	case WSAEWOULDBLOCK: // WSA
		throw suc::SucWSAException("Nonblocking operation cannot be completed immediately.");
		break;
	case WSAEINVAL: // WSA
		throw suc::SucWSAException("Invalid value or invalid state of the socket.");
		break;
	case WSAENOTCONN: // Socket
		throw suc::SucSocketException("The socket is not connected.");
		break;
	case WSAEADDRNOTAVAIL: // Socket
		throw suc::SucSocketException("The specified address is not valid in its context.");
		break;
	case WSAEAFNOSUPPORT: // Socket
		throw suc::SucSocketException("Address family not supported by protocol family.");
		break;
	case WSAECONNREFUSED: // Socket
		throw suc::SucSocketException("Connection refused.");
		break;
	case WSAENETRESET: // Socket
		throw suc::SucSocketException("Connection broken due to keep-alive activity indicating a failure.");
		break;
	case WSAESHUTDOWN: // Socket
		throw suc::SucSocketException("The used direction (send/receive) has already been shut down.");
		break;
	case WSAEMSGSIZE: // Socket
		throw suc::SucSocketException("The message was too large to fit into the buffer.");
		break;
	case WSAEISCONN: // Socket
		throw suc::SucSocketException("The socket is already connected.");
		break;
	case WSAECONNABORTED: // Socket
		throw suc::SucSocketException("Software cause connection abort. Possibly transmission timeout or protocol error.");
		break;
	case WSAENETUNREACH: // Socket
		throw suc::SucSocketException("Unreachable host.");
		break;
	case WSAETIMEDOUT: // Socket
		throw suc::SucSocketException("Connection timeout.");
		break;
	case WSAECONNRESET: // Socket
		throw suc::SucSocketException("Connection reset.");
		break;
	case WSAEINTR: // Runtime
		throw suc::SucRuntimeException("Interrupted function call. A blocking operation was interrupted by a call to WSACancelBlockingCall.");
		break;

	default:
		throw suc::SucException("Unknown WSA error code: " + std::to_string(err_code));
		break;
	}
}



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
		sucHandleErrorCode(iResult);
	}

	return result;
}



#endif