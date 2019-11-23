// ---------------------------------------------------------------------------------------- //
//																							//
// WELCOME TO THE SUC!																		//
// (Socket Utility Cpp)																		//
//																							//
// This thing tries to simplify the shitty WinSock-lowlevel-trash.							//
//																							//
// Conventions:																				//
//		- function names are suc*															//
//		- class names are Suc*																//
//		- macro names are SUC_*																//
//																							//
// ----------------------------------------------------------------------------------------	//



#pragma once
#ifndef SOCKETUTILITY_H
#define SOCKETUTILITY_H

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <optional>
#include <cstdint>


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

	constexpr auto SUC_IPV4 = AF_INET;
	constexpr auto SUC_IPV6 = AF_INET6;
	constexpr auto SUC_IPVX = AF_UNSPEC;

	constexpr auto SUC_ADDR_LOCAL_HOST = "127.0.0.1";
	constexpr auto SUC_ADDR_ANY = "0.0.0.0";

	constexpr auto SUC_TIMEOUT_NEVER = (-1);
	constexpr auto SUC_TIMEOUT_INSTANT = 0;


//#pragma warning (disable: 4505)


	// ------------------------------------------------ //
	//					SUC Exceptions					//
	//													//
	// I had to make my own exceptions to feel cool.	//
	// ------------------------------------------------ //
	class SucException
	{
	public:
		explicit SucException(std::string msg = "") : msg(std::move(msg)) {}
		void print() const
		{
			std::cout << "Exception: " << msg << std::endl;
		}
		[[nodiscard]] const std::string& getMsg() const
		{
			return msg;
		}
	private:
		std::string msg;
	};

	class SucRuntimeException : public SucException
	{
	public:
		explicit SucRuntimeException(std::string msg = "") : SucException(std::move(msg)) {}
	};

	class SucSocketException : public SucException
	{
	public:
		explicit SucSocketException(std::string msg = "") : SucException(std::move(msg)) {}
	};

	class SucSystemException : public SucException
	{
	public:
		explicit SucSystemException(std::string msg = "") : SucException(std::move(msg)) {}
	};

	class SucWSAException : public SucSystemException
	{
	public:
		explicit SucWSAException(std::string msg = "") : SucSystemException(std::move(msg)) {}
	};

	class SucInvalidValueException : public SucException
	{
	public:
		explicit SucInvalidValueException(std::string msg = "") : SucException(std::move(msg)) {}
	};

	class SucMemoryError : public SucException
	{
	public:
		explicit SucMemoryError(std::string msg = "") : SucException(std::move(msg)) {}
	};




	// ---------------------------- //
	//								//
	//		  SUC functions			//
	//								//
	// ---------------------------- //


	/* Split a string */
	static std::vector<std::string> sucSplitString(const std::string& to_str, const char delimiter)
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

} // namespace suc



#endif // File guard