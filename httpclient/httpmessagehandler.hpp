#ifndef HTTPMESSAGEHANDLER_HPP
#define HTTPMESSAGEHANDLER_HPP

#include "httprequestmessage.hpp"
#include "httpresponsemessage.hpp"

class HttpMessageHandler
{
public:
    HttpMessageHandler();

    std::shared_ptr<HttpResponseMessage> Send(
        const std::shared_ptr<HttpRequestMessage> &request);

private:
};

#endif // HTTPMESSAGEHANDLER_HPP
