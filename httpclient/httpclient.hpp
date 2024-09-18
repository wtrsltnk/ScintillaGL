#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

#include "httpcontent.hpp"
#include "httpmessagehandler.hpp"
#include "httpresponsemessage.hpp"
#include <map>
#include <memory>
#include <string>

class HttpClient
{
public:
    HttpClient();

    HttpClient(
        const std::shared_ptr<HttpMessageHandler> &messageHandler);

    virtual ~HttpClient();

public:
    std::string BaseAddress;
    std::map<std::string, std::string> DefaultRequestHeaders;

public:
    std::shared_ptr<HttpResponseMessage> Delete(
        const std::string &uri);

    std::shared_ptr<HttpResponseMessage> Get(
        const std::string &uri);

    std::shared_ptr<HttpResponseMessage> Patch(
        const std::string &uri,
        const std::shared_ptr<HttpContent> &content);

    std::shared_ptr<HttpResponseMessage> Post(
        const std::string &uri,
        const std::shared_ptr<HttpContent> &content);

    std::shared_ptr<HttpResponseMessage> Put(
        const std::string &uri,
        const std::shared_ptr<HttpContent> &content);

private:
    std::shared_ptr<HttpMessageHandler> _messageHandler;
};

#endif // HTTPCLIENT_HPP
