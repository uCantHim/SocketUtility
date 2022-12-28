#pragma once

// ---------------------------------------------------------------------------------------- //
//                                                                                          //
// WELCOME TO THE SUC!                                                                      //
// (Socket Utility Cpp)                                                                     //
//                                                                                          //
// This thing tries to simplify the shitty WinSock-lowlevel-trash.                          //
// Now with Linux support! (semi and untested)                                              //
//                                                                                          //
// ---------------------------------------------------------------------------------------- //

#include <cstdint>
#include <stdexcept>

#include "os.h"

namespace suc
{
    enum class AddressType : int
    {
        eIPv4 = AF_INET,
        eIPv6 = AF_INET6,
        eUnspecified = AF_UNSPEC,
    };

    inline auto to_string(AddressType family) -> std::string
    {
        switch (family)
        {
        case AddressType::eIPv4: return "IPv4";
        case AddressType::eIPv6: return "IPv6";
        case AddressType::eUnspecified: return "unspecified";
        }

        throw std::logic_error("[In to_string(Family)]: Enum options exhausted");
    }

    struct Timeout
    {
        static constexpr auto instant() -> Timeout { return 0; }
        static constexpr auto never() -> Timeout { return -1; }

        constexpr Timeout() = default;
        constexpr Timeout(int milliseconds) : timeoutMs(milliseconds) {}

        constexpr auto operator<=>(const Timeout&) const = default;

        int timeoutMs{ never().timeoutMs };
    };

    namespace addr
    {
        constexpr auto localhost = "127.0.0.1";
        constexpr auto localhost6 = "::1";
        constexpr auto all = "0.0.0.0";
    } // namespace addr

    class Error : public std::exception
    {
    public:
        explicit Error(std::string msg = "") : msg(std::move(msg)) {}

        [[nodiscard]]
        auto what() const noexcept -> const char* override
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
} // namespace suc
