#ifndef HTTPREQUESTMESSAGE_HPP
#define HTTPREQUESTMESSAGE_HPP

#include "httpcontent.hpp"
#include "httpmethod.hpp"
#include <map>
#include <memory>
#include <string>

class HttpRequestMessage
{
public:
    HttpRequestMessage();

    HttpRequestMessage(
        HttpMethod method);

    HttpRequestMessage(
        HttpMethod method,
        const std::string &requestUri);

    virtual ~HttpRequestMessage();

public:
    std::shared_ptr<HttpContent> Content;
    const HttpMethod Method = HttpMethod::Get;
    const std::string RequestUri;

public:
    const std::map<std::string, std::string> &Headers() const;
};

#endif // HTTPREQUESTMESSAGE_HPP
