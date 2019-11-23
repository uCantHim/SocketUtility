#include "HttpServer.h"

#include "Async.h"



suc::HttpServer::HttpServer(int port)
{
	auto func = std::bind(&HttpServer::handleConnection, this, std::placeholders::_1);
	AsyncServer::create(port, SUC_IPV4, func);
}


void suc::HttpServer::handleConnection(AsyncClient* newClient)
{
}
