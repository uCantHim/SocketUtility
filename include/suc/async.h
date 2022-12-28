#pragma once

#include <functional>

#include "client_socket.h"
#include "server_socket.h"
#include "util.h"

namespace suc
{
    class AsyncServer
    {
    public:
        AsyncServer(int port, AddressType family);
        AsyncServer(AsyncServer&&) noexcept = default;
        ~AsyncServer() noexcept;

        AsyncServer(const AsyncServer&) = delete;
        AsyncServer& operator=(const AsyncServer&) = delete;
        AsyncServer& operator=(AsyncServer&&) noexcept = delete;

        /**
         * Starts the server.
         * The server runs in a detached thread.
         *
         * The server waits for incoming connections and passes them to the onConnection callback.
         * When an error occurs that requires the server to terminate, onError is called and the
         * server is closed.
         *
         * Calling start() while the server is running does nothing. Calling start() after the server
         * has been stopped (or an error has occured for that matter) restarts the server on the same
         * port and with the same family.
         *
         * Throws an exception if connect() or bind() of the underlying socket fail.
         *
         * @throw suc::suc_error
         */
        void start();

        /**
         * Stops the server.
         */
        void stop();

        /**
         * Called when a client connects to the server.
         *
         * @param ClientSocket newClient The connecting client
         */
        void onConnection(std::function<void(ClientSocket)> f);

        /**
         * Called when an error occurs during server execution.
         *
         * @param const suc_error& The exception that occured in the server thread
         */
        void onError(std::function<void(const Error&)> f);

        /**
         * Called when the server thread terminates.
         */
        void onTerminate(std::function<void(void)> f);

    private:
        const int port;
        const AddressType family;

        std::function<void(ClientSocket)> onConnectionFunc;
        std::function<void(const Error&)> onErrorFunc;
        std::function<void()> onTerminateFunc;

        ServerSocket socket{};
        bool shouldClose{ false }; // Indicates whether stop() has been called or an error occured
        bool isRunning{ false };
    };
} // namespace suc
