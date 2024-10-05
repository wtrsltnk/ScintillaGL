#include "httprequestmessage.hpp"

HttpRequestMessage::HttpRequestMessage() = default;

HttpRequestMessage::HttpRequestMessage(
    HttpMethod method)
    : Method(method)
{}

HttpRequestMessage::HttpRequestMessage(
    HttpMethod method,
    const std::string &requestUri)
    : Method(method), RequestUri(requestUri)
{}

HttpRequestMessage::~HttpRequestMessage() = default;

const std::map<std::string, std::string> &HttpRequestMessage::Headers() const
{
    if (Content == nullptr)
    {
        static std::map<std::string, std::string> empty;
        return empty;
    }

    return Content->Headers;
}
