#pragma once
#ifndef HHTPSERVER_H
#define HTTPSERVER_H

#include <list>
#include <functional>

namespace suc
{
	class AsyncClient;

	class HttpRequest
	{

	};

	class HttpResponse
	{

	};

	class HttpServer
	{
	public:
		using RequestCallback = std::function<HttpResponse(HttpRequest)>;

		HttpServer(int port);

		void setRoute(const std::string& url, std::function<HttpResponse> callback);
		void setDefaultCallback(RequestCallback callback);

	private:
		void handleConnection(AsyncClient* newClient);
		auto handleRequest(HttpRequest request) -> HttpResponse;

		RequestCallback defaultCallback{
			[](HttpRequest) -> HttpResponse { return {}; }
		};
	};
} // namespace suc



#endif