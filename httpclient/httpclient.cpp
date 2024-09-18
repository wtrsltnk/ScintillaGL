#include "httpclient.hpp"

HttpClient::HttpClient() = default;

HttpClient::HttpClient(
    const std::shared_ptr<HttpMessageHandler> &messageHandler)
    : _messageHandler(messageHandler)
{}

HttpClient::~HttpClient() = default;

std::shared_ptr<HttpResponseMessage> HttpClient::Delete(
    const std::string &uri)
{
    auto requestMessage = std::make_shared<HttpRequestMessage>(HttpMethod::Delete, uri);

    return _messageHandler->Send(requestMessage);
}

std::shared_ptr<HttpResponseMessage> HttpClient::Get(
    const std::string &uri)
{
    auto requestMessage = std::make_shared<HttpRequestMessage>(HttpMethod::Get, uri);

    return _messageHandler->Send(requestMessage);
}

std::shared_ptr<HttpResponseMessage> HttpClient::Patch(
    const std::string &uri,
    const std::shared_ptr<HttpContent> &content)
{
    auto requestMessage = std::make_shared<HttpRequestMessage>(HttpMethod::Patch, uri);

    requestMessage->Content = content;

    return _messageHandler->Send(requestMessage);
}

std::shared_ptr<HttpResponseMessage> HttpClient::Post(
    const std::string &uri,
    const std::shared_ptr<HttpContent> &content)
{
    auto requestMessage = std::make_shared<HttpRequestMessage>(HttpMethod::Post, uri);

    requestMessage->Content = content;

    return _messageHandler->Send(requestMessage);
}

std::shared_ptr<HttpResponseMessage> HttpClient::Put(
    const std::string &uri,
    const std::shared_ptr<HttpContent> &content)
{
    auto requestMessage = std::make_shared<HttpRequestMessage>(HttpMethod::Put, uri);

    requestMessage->Content = content;

    return _messageHandler->Send(requestMessage);
}
