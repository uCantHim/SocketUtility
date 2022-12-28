#pragma once

#include <string>
#include <vector>

#include "util.h"

namespace suc
{
    class ClientSocket
    {
    public:
        ClientSocket() = default;
        explicit ClientSocket(SOCKET socket) noexcept;

        ClientSocket(const ClientSocket&) = delete;
        ClientSocket(ClientSocket&& other) noexcept;
        ~ClientSocket() noexcept;

        ClientSocket& operator=(const ClientSocket&) = delete;
        ClientSocket& operator=(ClientSocket&& rhs) noexcept;

        static constexpr size_t kDefaultRecvSize = 1024;

        /**
         * Attempt to connect to a remote server.
         *
         * @param std::string ip The server's IP address. An empty string
         *                       or the string "localhost" is interpreted
         *                       as the localhost address.
         * @param int port The server's port
         * @param AddressType family
         *
         * @return bool True if the connection was successfully established, false otherwise.
         *
         * @throw Error
         */
        bool connect(const std::string& ip, int port, AddressType family = AddressType::eIPv4);

        /**
         * Send data through the socket. This is the classic c-style signature version.
         *
         * @param const void* buf  The data to be sent
         * @param size_t      size The size of the buffer
         *
         * @throw Error
         */
        void send(const void* buf, size_t size) const;

        /**
         * Send data through the socket.
         *
         * @param const std::vector<sbyte>& buf
         *
         * @throw Error
         */
        void send(const std::vector<std::byte>& buf) const;

        /**
         * Send data through the socket.
         *
         * @param const std::string& str
         *
         * @throw Error
         */
        void send(const std::string& str) const;

        /**
         * @brief Read data from the socket
         *
         * @param void*  buf
         * @param size_t maxBytes Maximum number of bytes to read. `buf`
         *                        must be at least `maxBytes` large.
         *
         * @return size_t number of bytes received
         */
        [[nodiscard]]
        auto recv(void* buf, size_t maxBytes, Timeout timeout = Timeout::never()) const -> size_t;

        /**
         * @brief Read data from the socket
         *
         * @param Timeout timeout Specifies the time in milliseconds that
         *                        the socket will wait for incoming data.
         *
         * @return std::vector<std::byte> The received data. Is empty if
         *         the timeout has expired and no data has been received.
         *
         * @throw Error
         */
        [[nodiscard]]
        auto recv(size_t maxBytes = kDefaultRecvSize,
                  Timeout timeout = Timeout::never()) const
            -> std::vector<std::byte>;

        /**
         * Read data from the socket.
         *
         * @param int timeout: Specifies the time in milliseconds that the
         *                     socket will wait for incoming data.
         *
         * @return std::string The received data as a string. Is empty if
         *                     the timeout has expired and no data has been
         *                     received.
         *
         * @throw Error
         */
        [[nodiscard]]
        auto recvString(size_t maxBytes = kDefaultRecvSize,
                        Timeout timeout = Timeout::never()) const
            -> std::string;

        /**
         * Tests if the socket has data ready to read.
         *
         * @param int timeout Specifies the time in milliseconds that the
         *                    method waits for data to become available.
         *
         * @return bool True if data is available, false otherwise. Is also
         *              true if the connection has been closed remotely. If
         *              this is the case, the next call to recv() or
         *              recvString() returns nothing.
         *
         * @throw Error
         */
        [[nodiscard]]
        bool hasData(Timeout timeout = Timeout::never()) const;

        /**
         * @brief Close the socket
         *
         * @throw Error
         */
        void close();

        /**
         * @brief Query whether the socket has been closed
         *
         * @return bool True if the socket is closed, false othwerwise
         */
        [[nodiscard]]
        bool isClosed() const noexcept;

    private:
        SOCKET socket{ INVALID_SOCKET };
        bool _isClosed{ true };
    };
} // namespace suc
