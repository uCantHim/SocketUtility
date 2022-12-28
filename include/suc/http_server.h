#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "async.h"
#include "util.h"

namespace suc
{
    class ClientSocket;

    constexpr auto CRLF = "\r\n";
    constexpr auto HTTP_VERSION_1_1 = "HTTP/1.1";

    enum class HttpStatusCode {
        eContinue                        = 100,
        eSwitchingProtocols                = 101,
        eOk                                = 200,
        eCreated                            = 201,
        eAccepted                        = 202,
        eNonAuthorativeInformation        = 203,
        eNoContent                        = 204,
        eResetContent                    = 205,
        ePartialContent                    = 206,
        eMultipleChoices                = 300,
        eMovedPermanently                = 301,
        eFound                            = 302,
        eSeeOther                        = 303,
        eNotModified                    = 304,
        eUseProxy                        = 305,
        eTemporaryRedirect                = 307,
        eBadRequest                        = 400,
        eUnauthorized                    = 401,
        ePaymentRequired                = 402,
        eForbidden                        = 403,
        eNotFound                        = 404,
        eMethodNotAllowed                = 405,
        eNotAcceptable                    = 406,
        eProxyAuthenticationRequired    = 407,
        eRequestTimeout                    = 408,
        eConflict                        = 409,
        eGone                            = 410,
        eLengthRequired                    = 411,
        ePreconditionFailed                = 412,
        eRequestEntityTooLarge        = 413,
        eRequestUriTooLarge            = 414,
        eUnsupportedMediaType            = 415,
        eRequestedRangeNotSatisfiable = 416,
        eExpectationFailed                = 417,
        eImATeapot                        = 418,
        eInternalServerError            = 500,
        eNotImplemented                    = 501,
        eBadGateway                        = 502,
        eServiceUnavailable                = 503,
        eGatewayTimeout                    = 504,
        eHttpVersionNotSupported        = 505
    };

    const std::unordered_map<HttpStatusCode, std::string_view> statusCodeStrings = {
        { HttpStatusCode::eContinue,                     "Continue" },
        { HttpStatusCode::eSwitchingProtocols,           "Switching Protocols" },
        { HttpStatusCode::eOk,                           "OK" },
        { HttpStatusCode::eCreated,                      "Created" },
        { HttpStatusCode::eAccepted,                     "Accepted" },
        { HttpStatusCode::eNonAuthorativeInformation,    "Non-Authorative Information" },
        { HttpStatusCode::eNoContent,                    "No Content" },
        { HttpStatusCode::eResetContent,                 "Reset Content" },
        { HttpStatusCode::ePartialContent,               "Partial Content" },
        { HttpStatusCode::eMultipleChoices,              "Multiple Choices" },
        { HttpStatusCode::eMovedPermanently,             "Moved Permanently" },
        { HttpStatusCode::eFound,                        "Found" },
        { HttpStatusCode::eSeeOther,                     "See Other" },
        { HttpStatusCode::eNotModified,                  "Not Modified" },
        { HttpStatusCode::eUseProxy,                     "Use Proxy" },
        { HttpStatusCode::eTemporaryRedirect,            "Temporary Redirect" },
        { HttpStatusCode::eBadRequest,                   "Bad Request" },
        { HttpStatusCode::eUnauthorized,                 "Unauthorized" },
        { HttpStatusCode::ePaymentRequired,              "Payment Required" },
        { HttpStatusCode::eForbidden,                    "Forbidden" },
        { HttpStatusCode::eNotFound,                     "Not Found" },
        { HttpStatusCode::eMethodNotAllowed,             "Method Not Allowed" },
        { HttpStatusCode::eNotAcceptable,                "Not Acceptable" },
        { HttpStatusCode::eProxyAuthenticationRequired,  "Proxy Authentication Required" },
        { HttpStatusCode::eRequestTimeout,               "Request Time-out" },
        { HttpStatusCode::eConflict,                     "Conflict" },
        { HttpStatusCode::eGone,                         "Gone" },
        { HttpStatusCode::eLengthRequired,               "Length Required" },
        { HttpStatusCode::ePreconditionFailed,           "Precondition Failed" },
        { HttpStatusCode::eRequestEntityTooLarge,        "Request Entity Too Large" },
        { HttpStatusCode::eRequestUriTooLarge,           "Request-URI Too Large" },
        { HttpStatusCode::eUnsupportedMediaType,         "Unsupported Media Type" },
        { HttpStatusCode::eRequestedRangeNotSatisfiable, "Requested range not satisfiable" },
        { HttpStatusCode::eExpectationFailed,            "Expectation Failed" },
        { HttpStatusCode::eImATeapot,                    "I'm a teapot" },
        { HttpStatusCode::eInternalServerError,          "Internal Server Error" },
        { HttpStatusCode::eNotImplemented,               "Not Implemented" },
        { HttpStatusCode::eBadGateway,                   "Bad Gateway" },
        { HttpStatusCode::eServiceUnavailable,           "Service Unavailable" },
        { HttpStatusCode::eGatewayTimeout,               "Gateway Time-out" },
        { HttpStatusCode::eHttpVersionNotSupported,      "HTTP Version not supported" }
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

        explicit HttpResponse(HttpStatusCode status, Headers headers = {});

        void setStatusCode(HttpStatusCode newStatus);
        void setHeader(const header_type& header) noexcept;
        void setContent(std::string body);

        [[nodiscard]]
        auto getRaw() const noexcept -> std::string;
        void sendTo(ClientSocket& client);

    private:
        static constexpr auto RESPONSE_HTTP_VERSION = HTTP_VERSION_1_1;
        [[nodiscard]]
        auto makeStatusLine() const noexcept -> std::string;
        [[nodiscard]]
        static auto makeHeaderString(const header_type& header) noexcept -> std::string;

        HttpStatusCode status;
        Headers headers;
        std::string content;
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
        HttpRequest(ClientSocket& sender, RequestLine requestLine, Headers headers);

    public:
        enum class Method
        {
            eOptions,
            eGet, eHead,
            ePost, ePut, eDelete,
            eTrace, eConnect,
            extension
        };

        static auto parseRequest(const std::string& msg, ClientSocket& client)
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
        class InvalidHttpRequestException : public Error
        {
        public:
            explicit InvalidHttpRequestException(const std::string& msg = "")
                : Error(msg) {}
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

    /**
     * @brief An HTTP server
     */
    class HttpServer
    {
    public:
        HttpServer(const HttpServer&) = delete;
        HttpServer(HttpServer&&) noexcept = delete;
        HttpServer& operator=(const HttpServer&) = delete;
        HttpServer& operator=(HttpServer&&) noexcept = delete;

        explicit HttpServer(int port);
        ~HttpServer() noexcept;

    private:
        AsyncServer server;

        void handleConnection(ClientSocket newClient);
        void handleClientMessage(const std::vector<std::byte>& msg, ClientSocket& client);
        void handleClientTerminate(ClientSocket& client);
    };
} // namespace suc
