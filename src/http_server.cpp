#include "http_server.h"

#include <iostream>

#include "async.h"

/*
    All citations of the form

    >>> (rfc-section | [citation-source])
    citation
    <<<

    are taken from RFC-2616 (https://tools.ietf.org/html/rfc2616)
    unless otherwise stated.
*/



// ------------------------ //
//        Http request        //
// ------------------------ //

suc::HttpRequest::HttpRequest(ClientSocket& sender, RequestLine requestLine, Headers headers)
    :
    sender(&sender),
    requestLine(std::move(requestLine)),
    headers(std::move(headers))
{
}


auto suc::HttpRequest::parseRequest(const std::string& msg, ClientSocket& client) -> HttpRequest
{
    auto requestFields = splitString(msg, CRLF);

    /**
     * An Http-request MUST have at least a start-line and an empty line:
     * >>> 5
     * Request = Request-Line                    ; Section 5.1
     *           *(( general-header            ; Section 4.5
     *               | request-header            ; Section 5.3
     *               | entity-header ) CRLF)    ; Section 7.1
     *           CRLF
     *           [ message-body ]                ; Section 4.3
     * <<<
     */
    if (requestFields.size() < 2)
        throw InvalidHttpRequestException();

    auto currentLine = requestFields.begin();

    /**
     * Discard leading blank lines:
     * >>> 4.1
     * In the interest of robustness, servers SHOULD ignore any empty
     * line(s) received where a Request-Line is expected. In other words, if
     * the server is reading the protocol stream at the beginning of a
     * message and receives a CRLF first, it should ignore the CRLF.
     * <<<
     *
     * Even though:
     * >>> 4.1
     * Certain buggy HTTP/1.0 client implementations generate extra CRLF's
     * after a POST request. To restate what is explicitly forbidden by the
     * BNF, an HTTP/1.1 client MUST NOT preface or follow a request with an
     * extra CRLF.
     * <<<
     */
    while (currentLine->empty()) currentLine++;

    // Parse request-line:
    auto requestLine = parseRequestLine(*currentLine++);

    // Parse headers:
    Headers headers;
    for (; !currentLine->empty(); currentLine++) {
        headers.insert(parseHeader(*currentLine));
    }

    return HttpRequest(client, requestLine, headers);
}

auto suc::HttpRequest::getMethod() const noexcept -> Method
{
    return requestLine.method;
}

auto suc::HttpRequest::getPath() const noexcept -> std::string
{
    return requestLine.path;
}

bool suc::HttpRequest::hasOption(const std::string& key) const noexcept
{
    return requestLine.options.find(key) != requestLine.options.end();
}

auto suc::HttpRequest::getOption(const std::string& key) const noexcept -> std::optional<std::string>
{
    if (hasOption(key))
        return requestLine.options.at(key);

    return {};
}

bool suc::HttpRequest::hasHeader(const std::string& key) const noexcept
{
    return headers.find(key) != headers.end();
}

auto suc::HttpRequest::getHeader(const std::string& key) const noexcept -> std::optional<std::string>
{
    if (hasHeader(key))
        return headers.at(key);

    return {};
}

void suc::HttpRequest::respond(HttpResponse response)
{
    response.sendTo(sender);
}


auto suc::HttpRequest::parseRequestLine(const std::string& requestLine) -> RequestLine
{
    auto assertSize = [](const auto& vec, size_t size, const std::string& msg) {
        if (vec.size() != size) {
            throw InvalidHttpRequestException(msg);
        }
    };

    /**
     * >>> 5.1
     * Request-Line = Method SP Request-URI SP HTTP-Version CRLF
     * <<<
     */
    auto tokens = splitString(requestLine, ' ');
    assertSize(tokens, 3, "The request line MUST contain exactly 3 fields.");

    const auto& method    = tokens[0];
    const auto& uri        = tokens[1];
    const auto& version = tokens[2];

    /**
     * >>> 5.1.2
     * Request-URI = "*" | absoluteURI | abs_path | authority
     * [...]
     * The absoluteURI form is REQUIRED when the request is being made to a
     * proxy.
     * [...]
     * The authority form is only used by the CONNECT method (section 9.9).
     * [...]
     * The Request-URI is transmitted in the format specified in section
     * 3.2.1.
     * <<<
     *
     * >>> 3.2.2
     * http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]
     * <<<
     */
    auto splitPath = splitString(uri, '?');
    if (splitPath.empty() || splitPath.size() > 2)
        throw InvalidHttpRequestException("Missing path in request URI.");
    const auto& path = splitPath[0];

    // Parse options
    Options options;
    if (splitPath.size() == 2)
    {
        for (const auto& opt : splitString(splitPath[1], '&'))
        {
            auto splitOpt = splitString(opt, '=');
            assertSize(splitOpt, 2, "HTTP option must be a key-value pair separated by \"=\"");

            const auto& key = splitOpt[0];
            const auto& value = splitOpt[1];
            options.try_emplace(key, value);
        }
    }

    return {
        parseMethod(method),
        path,
        options,
        version
    };
}


auto suc::HttpRequest::parseMethod(const std::string& method) -> Method
{
    /**
     * >>> 5.1.1
     * Method = "OPTIONS"            ; Section 9.2
     *        | "GET"              ; Section 9.3
     *        | "HEAD"             ; Section 9.4
     *        | "POST"             ; Section 9.5
     *        | "PUT"              ; Section 9.6
     *        | "DELETE"           ; Section 9.7
     *        | "TRACE"            ; Section 9.8
     *        | "CONNECT"            ; Section 9.9
     *        | extension-method
     * extension-method = token
     * <<<
     */

    if (method == "OPTIONS") {
        return Method::eOptions;
    }
    if (method == "GET") {
        return Method::eGet;
    }
    if (method == "HEAD") {
        return Method::eHead;
    }
    if (method == "POST") {
        return Method::ePost;
    }
    if (method == "PUT") {
        return Method::ePut;
    }
    if (method == "DELETE") {
        return Method::eDelete;
    }
    if (method == "TRACE") {
        return Method::eTrace;
    }
    if (method == "CONNECT") {
        return Method::eConnect;
    }
    return Method::extension;
}


auto suc::HttpRequest::parseHeader(const std::string& headerLine) -> header_type
{
    /**
     * >>> 4.2
     * message-header = field-name ":" [ field-value ]
     * field-name     = token
     * field-value    = *( field-content | LWS )
     * field-content  = <the OCTETs making up the field-value
     *                 and consisting of either *TEXT or combinations
     *                 of token, separators, and quoted-string>
     * <<<
     */
    auto tokens = splitString(headerLine, ':');
    if (tokens.size() < 2)
        throw InvalidHttpRequestException("Invalid request header: " + headerLine);

    if (tokens.size() > 2)
    {
        // The value contained one or more colons, re-assemble the value.
        auto value = tokens[1];
        for (size_t i = 2; i < tokens.size(); i++)
            value += ":" + tokens[i];
        tokens[1] = value;
    }

    const auto& fieldName = tokens[0];
    const auto& fieldValue = tokens[1];

    return { fieldName, fieldValue };
}



// ------------------------ //
//        HTTP response        //
// ------------------------ //

suc::HttpResponse::HttpResponse(HttpStatusCode status, Headers headers)
    :
    status(status),
    headers(std::move(headers))
{
}


void suc::HttpResponse::setStatusCode(HttpStatusCode newStatus)
{
    status = newStatus;
}


void suc::HttpResponse::setHeader(const header_type& header) noexcept
{
    /**
     * >>> 4.4
     * The Content-Length header field MUST NOT be sent if [...] a
     * Transfer-Encoding header field is present [...].
     * If a message is received with both a Transfer-Encoding header
     * field and a Content-Length header field, the latter MUST be
     * ignored.
     * <<<
     */
    if (header.first == "Content-Length") {
        if (headers.find("Transfer-Encoding") != headers.end())
            return;
    }
    if (header.first == "Transfer-Encoding") {
        headers.erase("Content-Length");
    }

    headers[header.first] = header.second;
}


void suc::HttpResponse::setContent(std::string body)
{
    content = std::move(body);
    setHeader({ "Content-Length", std::to_string(content.size()) });
}


auto suc::HttpResponse::getRaw() const noexcept -> std::string
{
    /**
     * >>> 6.0
     * Response = Status-Line                    ; Section 6.1
     *            *(( general-header            ; Section 4.5
     *              | response-header            ; Section 6.2
     *              | entity-header ) CRLF)    ; Section 7.1
     *            CRLF
     *            [ message-body ]                ; Section 7.2
     * <<<
     */
    std::string result;

    result += makeStatusLine();
    result += CRLF;
    for (const auto& header : headers)
    {
        result += makeHeaderString(header);
        result += CRLF;
    }
    result += CRLF;
    result += content;

    return result;
}


void suc::HttpResponse::sendTo(ClientSocket& client)
{
    client.send(getRaw());
}


auto suc::HttpResponse::makeStatusLine() const noexcept -> std::string
{
    /**
     * >>> 6.1
     * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
     * <<<
     */
    std::string result;

    result += RESPONSE_HTTP_VERSION;
    result += " ";
    result += std::to_string(static_cast<unsigned int>(status));
    result += " ";
    result += statusCodeStrings.at(status);

    return result;
}


auto suc::HttpResponse::makeHeaderString(const header_type& header) noexcept -> std::string
{
    /**
     * >>> 4.2
     * message-header = field-name ":" [ field-value ]
     * field-name     = token
     * field-value    = *( field-content | LWS )
     * field-content  = <the OCTETs making up the field-value
     *                 and consisting of either *TEXT or combinations
     *                 of token, separators, and quoted-string>
     * <<<
     */
    std::string result;

    result += header.first;
    result += ": ";
    result += header.second;

    return result;
}



// ------------------------- //
//        Http server        //
// ------------------------- //

suc::HttpServer::HttpServer(int port)
    :
    server(port, AddressType::eIPv4)
{
    server.onConnection([this](ClientSocket newClient){ handleConnection(std::move(client)); });
    server.start();
}

suc::HttpServer::~HttpServer() noexcept
{
    server.stop();
}

void suc::HttpServer::handleConnection(ClientSocket newClient)
{
    using namespace std::placeholders;
    newClient->onMessage = std::bind(&HttpServer::handleClientMessage, this, _1, _2);
    newClient->onTerminate = std::bind(&HttpServer::handleClientTerminate, this, _1);
    std::cout << "--- New connection: client #" << newClient->getId() << "\n";
}

void suc::HttpServer::handleClientMessage(const std::vector<std::byte>& msg, ClientSocket& client)
{
    auto request = HttpRequest::parseRequest(msg, client);
    HttpResponse response(
        HttpStatusCode::eOk,
        {
            { "Content-Type", "text/html"}
        }
    );
    response.setContent("<html><head><title>Yo</title></head><body>Hello World!</body></html>");
    request.respond(response);
}

void suc::HttpServer::handleClientTerminate(ClientSocket& client)
{
    std::cout << "--- Disconnect: client #" << client->getId() << "\n";
}
