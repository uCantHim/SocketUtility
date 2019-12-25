#pragma once
#ifndef HHTPSERVER_H
#define HTTPSERVER_H

#include <string>
#include <list>
#include <unordered_map>
#include <functional>
#include <optional>
#include <pointers>

#include "ByteBuffer.h"

#undef DELETE // So I can use the identifier DELETE in HttpRequest::Method

namespace suc
{
	class ClientSocket;
	class AsyncClient;
	class AsyncServer;

	constexpr auto CRLF = "\r\n";
	constexpr auto HTTP_VERSION_1_1 = "HTTP/1.1";

	enum class HttpStatusCode {
		CONTINUE						= 100,
		SWITCHING_PROTOCOLS				= 101,
		OK								= 200,
		CREATED							= 201,
		ACCEPTED						= 202,
		NON_AUTHORATIVE_INFORMATION		= 203,
		NO_CONTENT						= 204,
		RESET_CONTENT					= 205,
		PARTIAL_CONTENT					= 206,
		MULTIPLE_CHOICES				= 300,
		MOVED_PERMANENTLY				= 301,
		FOUND							= 302,
		SEE_OTHER						= 303,
		NOT_MODIFIED					= 304,
		USE_PROXY						= 305,
		TEMPORARY_REDIRECT				= 307,
		BAD_REQUEST						= 400,
		UNAUTHORIZED					= 401,
		PAYMENT_REQUIRED				= 402,
		FORBIDDEN						= 403,
		NOT_FOUND						= 404,
		METHOD_NOT_ALLOWED				= 405,
		NOT_ACCEPTABLE					= 406,
		PROXY_AUTHENTICATION_REQUIRED	= 407,
		REQUEST_TIMEOUT					= 408,
		CONFLICT						= 409,
		GONE							= 410,
		LENGTH_REQUIRED					= 411,
		PRECONDITION_FAILED				= 412,
		REQUEST_ENTITY_TOO_LARGE		= 413,
		REQUEST_URI_TOO_LARGE			= 414,
		UNSUPPORTED_MEDIA_TYPE			= 415,
		REQUESTED_RANGE_NOT_SATISFIABLE = 416,
		EXPECTATION_FAILED				= 417,
		IM_A_TEAPOT						= 418,
		INTERNAL_SERVER_ERROR			= 500,
		NOT_IMPLEMENTED					= 501,
		BAD_GATEWAY						= 502,
		SERVICE_UNAVAILABLE				= 503,
		GATEWAY_TIMEOUT					= 504,
		HTTP_VERSION_NOT_SUPPORTED		= 505
	};

	const std::unordered_map<HttpStatusCode, std::string> statusCodeStrings = {
		{ HttpStatusCode::CONTINUE,							"Continue" },
		{ HttpStatusCode::SWITCHING_PROTOCOLS,				"Switching Protocols" },
		{ HttpStatusCode::OK,								"OK" },
		{ HttpStatusCode::CREATED,							"Created" },
		{ HttpStatusCode::ACCEPTED,							"Accepted" },
		{ HttpStatusCode::NON_AUTHORATIVE_INFORMATION,		"Non-Authorative Information" },
		{ HttpStatusCode::NO_CONTENT,						"No Content" },
		{ HttpStatusCode::RESET_CONTENT,					"Reset Content" },
		{ HttpStatusCode::PARTIAL_CONTENT,					"Partial Content" },
		{ HttpStatusCode::MULTIPLE_CHOICES,					"Multiple Choices" },
		{ HttpStatusCode::MOVED_PERMANENTLY,				"Moved Permanently" },
		{ HttpStatusCode::FOUND,							"Found" },
		{ HttpStatusCode::SEE_OTHER,						"See Other" },
		{ HttpStatusCode::NOT_MODIFIED,						"Not Modified" },
		{ HttpStatusCode::USE_PROXY,						"Use Proxy" },
		{ HttpStatusCode::TEMPORARY_REDIRECT,				"Temporary Redirect" },
		{ HttpStatusCode::BAD_REQUEST,						"Bad Request" },
		{ HttpStatusCode::UNAUTHORIZED,						"Unauthorized" },
		{ HttpStatusCode::PAYMENT_REQUIRED,					"Payment Required" },
		{ HttpStatusCode::FORBIDDEN,						"Forbidden" },
		{ HttpStatusCode::NOT_FOUND,						"Not Found" },
		{ HttpStatusCode::METHOD_NOT_ALLOWED,				"Method Not Allowed" },
		{ HttpStatusCode::NOT_ACCEPTABLE,					"Not Acceptable" },
		{ HttpStatusCode::PROXY_AUTHENTICATION_REQUIRED,	"Proxy Authentication Required" },
		{ HttpStatusCode::REQUEST_TIMEOUT,					"Request Time-out" },
		{ HttpStatusCode::CONFLICT,							"Conflict" },
		{ HttpStatusCode::GONE,								"Gone" },
		{ HttpStatusCode::LENGTH_REQUIRED,					"Length Required" },
		{ HttpStatusCode::PRECONDITION_FAILED,				"Precondition Failed" },
		{ HttpStatusCode::REQUEST_ENTITY_TOO_LARGE,			"Request Entity Too Large" },
		{ HttpStatusCode::REQUEST_URI_TOO_LARGE,			"Request-URI Too Large" },
		{ HttpStatusCode::UNSUPPORTED_MEDIA_TYPE,			"Unsupported Media Type" },
		{ HttpStatusCode::REQUESTED_RANGE_NOT_SATISFIABLE,	"Requested range not satisfiable" },
		{ HttpStatusCode::EXPECTATION_FAILED,				"Expectation Failed" },
		{ HttpStatusCode::IM_A_TEAPOT,						"I'm a teapot" },
		{ HttpStatusCode::INTERNAL_SERVER_ERROR,			"Internal Server Error" },
		{ HttpStatusCode::NOT_IMPLEMENTED,					"Not Implemented" },
		{ HttpStatusCode::BAD_GATEWAY,						"Bad Gateway" },
		{ HttpStatusCode::SERVICE_UNAVAILABLE,				"Service Unavailable" },
		{ HttpStatusCode::GATEWAY_TIMEOUT,					"Gateway Time-out" },
		{ HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED,		"HTTP Version not supported" }
	};

	/*
	A response to a HTTP-Request. */
	class HttpResponse
	{
	public:
		using str_str_map = std::unordered_map<std::string, std::string>;
		using str_str_pair = std::pair<std::string, std::string>;
		using Options = str_str_map;
		using Headers = str_str_map;
		using option_type = str_str_pair;
		using header_type = str_str_pair;

		explicit HttpResponse(HttpStatusCode status = HttpStatusCode::OK, Headers headers = {});

		void setStatusCode(HttpStatusCode newStatus);
		void setHeader(const header_type& header) noexcept;
		void setContent(std::string body);
		void setContent(ByteBuffer body);

		[[nodiscard]]
		auto getRaw() const noexcept -> std::string;
		void sendTo(gsl::not_null<ClientSocket*> client);

	private:
		static constexpr auto RESPONSE_HTTP_VERSION = HTTP_VERSION_1_1;
		[[nodiscard]]
		auto makeStatusLine() const noexcept -> std::string;
		[[nodiscard]]
		static auto makeHeaderString(const header_type& header) noexcept -> std::string;

		HttpStatusCode status;
		Headers headers;
		ByteBuffer content;
	};

	/*
	A HTTP-Request. */
	class HttpRequest
	{
	public:
		using str_str_map = std::unordered_map<std::string, std::string>;
		using str_str_pair = std::pair<std::string, std::string>;
		using Options = str_str_map;
		using Headers = str_str_map;
		using option_type = str_str_pair;
		using header_type = str_str_pair;

	private:
		struct RequestLine;
		HttpRequest(gsl::not_null<ClientSocket*> sender, RequestLine requestLine, Headers headers);

	public:
		enum class Method {
			OPTIONS,
			GET, HEAD,
			POST, PUT, DELETE,
			TRACE, CONNECT,
			extension
		};

		static auto parseRequest(const ByteBuffer& msg, gsl::not_null<ClientSocket*> client)
			-> HttpRequest;

		[[nodiscard]]
		auto getMethod() const noexcept -> Method;

		[[nodiscard]]
		auto getPath() const noexcept -> std::string;

		[[nodiscard]]
		bool hasOption(const std::string& key) const noexcept;
		[[nodiscard]]
		auto getOption(const std::string& key) const noexcept -> std::optional<std::string>;

		[[nodiscard]]
		bool hasHeader(const std::string& key) const noexcept;
		[[nodiscard]]
		auto getHeader(const std::string& key) const noexcept -> std::optional<std::string>;

		void respond(HttpResponse response);

	private:
		/*
		Thrown from HttpRequest::parseRequest when the provided ByteBuffer
		contains no valid HTTP-Request. */
		class InvalidHttpRequestException : public SucException
		{
		public:
			explicit InvalidHttpRequestException(const std::string& msg = "")
				: SucException(msg) {}
		};

		struct RequestLine {
			Method method;
			std::string path;
			Options options;
			std::string version;
		};
		static auto parseRequestLine(const std::string& requestLine) -> RequestLine;
		static auto parseMethod(const std::string& method) -> Method;
		static auto parseHeader(const std::string& headerLine) -> header_type;

		ClientSocket* sender;
		const RequestLine requestLine;
		const Headers headers;
	};

	/*
	A HTTP server. */
	class HttpServer
	{
	public:
		explicit HttpServer(int port);
		HttpServer(const HttpServer&) = default;
		HttpServer(HttpServer&&) noexcept = default;
		~HttpServer() noexcept;

		HttpServer& operator=(const HttpServer&) = default;
		HttpServer& operator=(HttpServer&&) noexcept = default;

	private:
		AsyncServer* server;

		void handleConnection(AsyncClient* newClient);
		void handleClientMessage(const ByteBuffer& msg, gsl::not_null<ClientSocket*> client);
		void handleClientTerminate(AsyncClient* client);
	};
} // namespace suc



#endif