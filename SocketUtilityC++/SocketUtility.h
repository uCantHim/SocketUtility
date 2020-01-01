#pragma once
#ifndef SOCKETUTILITY_H
#define SOCKETUTILITY_H

// ---------------------------------------------------------------------------------------- //
//																							//
// WELCOME TO THE SUC!																		//
// (Socket Utility Cpp)																		//
//																							//
// This thing tries to simplify the shitty WinSock-lowlevel-trash.							//
// Now with Linux support! (semi and untested)												//
//																							//
// Conventions:																				//
//		- function names are suc*															//
//		- class names are Suc*																//
//		- macro names are SUC_*																//
//																							//
// ----------------------------------------------------------------------------------------	//


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
	#include <netdb.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <netinet/in.h>
	#include <arpa/inet.h> // Might fix some segfault
	#include <unistd.h> // write() and read()
#endif

// Datatype defines
#ifdef OS_IS_WINDOWS
	//#define SOCKET SOCKET
	//#define INVALID_SOCKET INVALID_SOCKET
#endif

#ifdef OS_IS_LINUX
	#define SOCKET int
	#define INVALID_SOCKET (-1)
	#define SOCKET_ERROR (-1)
#endif

#include <cstdint>
#include <cassert>
#include <cstring> // memset()
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <optional>


namespace suc
{
	// ---------------------------------------------------------------- //
	//							SUC #defines							//
	//																	//
	// These try to be more intuitive than the WinSock definitions.		//
	// ---------------------------------------------------------------- //

	using ubyte = std::uint8_t; // unsigned byte
	using sbyte = char;	// signed byte
	using uint = std::uint32_t;
	using ulong = std::uint64_t;

	constexpr auto IPV4 = AF_INET;
	constexpr auto IPV6 = AF_INET6;
	constexpr auto IPVX = AF_UNSPEC;

	constexpr auto ADDR_LOCALHOST_4 = "127.0.0.1";
	constexpr auto ADDR_LOCALHOST_6 = "::1";
	constexpr auto ADDR_ALL = "0.0.0.0";

	constexpr auto TIMEOUT_NEVER = -1;
	constexpr auto TIMEOUT_INSTANT = 0;


	// ------------------------------------------------ //
	//					SUC Exceptions					//
	//													//
	// I had to make my own exceptions to feel cool.	//
	// ------------------------------------------------ //

	class suc_error : public std::exception
	{
	public:
		explicit suc_error(std::string msg = "") : msg(std::move(msg)) {}

		[[nodiscard]]
		auto what() const noexcept -> const char* override
		{
			return msg.c_str();
		}

	private:
		std::string msg;
	};

	class runtime_error : public suc_error
	{
	public:
		explicit runtime_error(std::string msg = "") : suc_error(std::move(msg)) {}
	};

	class network_error : public suc_error
	{
	public:
		explicit network_error(std::string msg = "") : suc_error(std::move(msg)) {}
	};

	class system_error : public suc_error
	{
	public:
		explicit system_error(std::string msg = "") : suc_error(std::move(msg)) {}
	};

	class value_error : public suc_error
	{
	public:
		explicit value_error(std::string msg = "") : suc_error(std::move(msg)) {}
	};

	class memory_error : public suc_error
	{
	public:
		explicit memory_error(std::string msg = "") : suc_error(std::move(msg)) {}
	};


	// ---------------------------- //
	//								//
	//		  SUC functions			//
	//								//
	// ---------------------------- //


	/* Split a string */
	static std::vector<std::string> splitString(const std::string& to_str, const char delimiter)
	{
		std::vector<std::string> result;
		std::stringstream _str;
		_str << to_str;
		std::string token;
		while (std::getline(_str, token, delimiter)) {
			result.push_back(token);
		}

		return result;
	}

	/* Split a string at a delimiting character sequence. */
	static std::vector<std::string> splitString(const std::string& str, const std::string& delimiter)
	{
		std::vector<std::string> result;

		auto i = str.begin();
		auto tokenStart = str.begin();
		for (; i != str.end(); i++)
		{
			for (auto delim = delimiter.begin(), temp = i;
				delim != delimiter.end() && temp != str.end();
				delim++, temp++)
			{
				if (*temp != *delim)
					break;
				if (delim == delimiter.end() - 1)
				{
					result.emplace_back(std::string(tokenStart, i));
					tokenStart = temp + 1;
					i = temp; // Not (temp + 1) here because of the i++ in for-loop
				}
			}
		}
		// Append the remainder of the input string
		auto remainingString = std::string(tokenStart, str.end());
		if (!remainingString.empty())
			result.push_back(remainingString);

		return result;
	}
} // namespace suc



#endif // File guard
