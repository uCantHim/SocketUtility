#include "Async.h"



// ------------------------ //
//		Server class		//
// ------------------------ //

suc::AsyncServer::AsyncServer(int port, int family, callback<ClientSocket> onConnection)
	:
	port(port),
	family(family),
	onConnectionFunc(std::move(onConnection)),
	onErrorFunc([](auto) {}),
	onTerminateFunc([]() {})
{
}


suc::AsyncServer::~AsyncServer() noexcept
{
	stop();
	while (isRunning); // Wait for the thread to terminate
}


void suc::AsyncServer::start()
{
	if (isRunning) return;

	socket.close();
	socket.bind(port, family);

	std::thread([&]() {
		isRunning = true;
		shouldClose = false;
		while (!socket.isClosed())
		{
			try {
				auto newClient = socket.accept();
				onConnectionFunc(std::move(newClient));
			}
			catch (const suc_error& err) {
				// It is normal for accept() to throw an error when the socket has been
				// closed. Only call onError when the socket hasn't been closed manually.
				if (!shouldClose)
				{
					stop();
					onErrorFunc(err);
				}
			}
		}
		onTerminateFunc();
		isRunning = false;
	}).detach();
}


void suc::AsyncServer::stop()
{
	shouldClose = true;
	socket.close();
}

void suc::AsyncServer::onConnection(std::function<void(ClientSocket)> f)
{
	onConnectionFunc = std::move(f);
}

void suc::AsyncServer::onError(std::function<void(const suc_error&)> f)
{
	onErrorFunc = std::move(f);
}

void suc::AsyncServer::onTerminate(std::function<void(void)> f)
{
	onTerminateFunc = std::move(f);
}