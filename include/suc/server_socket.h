#pragma once

#include <memory>

#include "util.h"

namespace suc
{
    class ClientSocket;

    /**
     * @brief A socker that accepts TCP incoming connections
     */
    class ServerSocket
    {
    public:
        ServerSocket() noexcept = default;

        /**
         * Effectively calls bind(port, family) after construction.
         *
         * @param int port   The port that the server will listen to
         * @param int family The IP family that the server will be compatible with
         *
         * @throw suc_error
         */
        explicit ServerSocket(int port, AddressType family = AddressType::eIPv4);

        ServerSocket(const ServerSocket&) = delete;
        ServerSocket(ServerSocket&& other) noexcept;

        ServerSocket& operator=(const ServerSocket&) = delete;
        ServerSocket& operator=(ServerSocket&& rhs) noexcept;

        /**
         * Closes the socket.
         */
        ~ServerSocket() noexcept;

        /**
         * Bind the server to localhost.
         *
         * @param int port   The port on which the server will listen
         * @param int family The IP family that the server will be compatible with. Must
         *                   be either suc::Family::eIPv4 or suc::Family::eIPv6.
         *                   TODO: suc::Family::eIPv6 is currently not available.
         *
         * @throw suc_error
         */
        void bind(int port, AddressType family = AddressType::eIPv4);

        /**
         * Wait for an incoming connection.
         *
         * This blocks the thread until a client has connected to the socket. Returns the new
         * client when a connection occurs.
         *
         * @return ClientSocket The new connection.
         *
         * @throw suc_error
         */
        [[nodiscard]]
        auto accept() const -> ClientSocket;

        /**
         * Close the socket.
         *
         * @throw suc_error
         */
        void close();

        /**
         * Tests if the socket is closed.
         *
         * This is not guaranteed to give correct results if the server has been closed because
         * of an earlier error.
         *
         * @return bool True if the socket has been closed manually, false otherwise.
         */
        [[nodiscard]]
        bool isClosed() const noexcept;

    private:
        bool _isClosed{ true };
        SOCKET socket{ INVALID_SOCKET };

        sockaddr_in address{};
    };
} // namespace suc
