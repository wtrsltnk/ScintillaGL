#include "httpmessagehandler.hpp"

#include "bytearraycontent.hpp"
#define WIN32_LEAN_AND_MEAN 1
#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <windows.h>
#include <wininet.h>

HttpMessageHandler::HttpMessageHandler() = default;

std::string GetHost(
    const std::string &url)
{
    size_t found = url.find_first_of(":");
    std::string protocol = url.substr(0, found);

    std::string url_new = url.substr(found + 3); // url_new is the url excluding the http part
    size_t found1 = url_new.find_first_of(":");
    std::string host = url_new.substr(0, found1);

    size_t found2 = url_new.find_first_of("/");
    std::string port = url_new.substr(found1 + 1, found2 - found1 - 1);
    std::string path = url_new.substr(found2);

    return host;
}

std::string GetPath(
    const std::string &url)
{
    size_t found = url.find_first_of(":");
    std::string protocol = url.substr(0, found);

    std::string url_new = url.substr(found + 3); // url_new is the url excluding the http part
    size_t found1 = url_new.find_first_of(":");
    std::string host = url_new.substr(0, found1);

    size_t found2 = url_new.find_first_of("/");
    std::string port = url_new.substr(found1 + 1, found2 - found1 - 1);
    std::string path = url_new.substr(found2);

    return path;
}

int GetPort(
    const std::string &url)
{
    size_t found = url.find_first_of(":");
    std::string protocol = url.substr(0, found);

    std::string url_new = url.substr(found + 3); // url_new is the url excluding the http part
    size_t found1 = url_new.find_first_of(":");
    std::string host = url_new.substr(0, found1);

    size_t found2 = url_new.find_first_of("/");
    std::string port = url_new.substr(found1 + 1, found2 - found1 - 1);
    std::string path = url_new.substr(found2);

    return std::stoi(port);
}

std::string GetHeaders(
    const std::shared_ptr<HttpRequestMessage> &request)
{
    std::stringstream ss;

    for (auto &header : request->Headers())
    {
        ss << header.first << ": " << header.second << "\n";
    }

    return ss.str();
}

const char *GetMethod(
    const std::shared_ptr<HttpRequestMessage> &request)
{
    if (request->Method == HttpMethod::Post)
    {
        return "POST";
    }

    if (request->Method == HttpMethod::Patch)
    {
        return "PATCH";
    }

    if (request->Method == HttpMethod::Put)
    {
        return "PUT";
    }

    return "GET";
}

std::shared_ptr<HttpResponseMessage> HttpMessageHandler::Send(
    const std::shared_ptr<HttpRequestMessage> &request)
{
    static HINTERNET hsession;
    hsession = InternetOpenA("wininet-test", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

    auto server = GetHost(request->RequestUri);

    HINTERNET hconnect = InternetConnect(
        hsession,
        server.c_str(),
        GetPort(request->RequestUri),
        NULL,
        NULL,
        INTERNET_SERVICE_HTTP,
        0,
        0);

    auto objectName = GetPath(request->RequestUri);

    const char *accept[] = {"text/xml", NULL};
    HINTERNET hrequest = HttpOpenRequest(
        hconnect,
        GetMethod(request),
        objectName.c_str(),
        NULL,
        NULL,
        accept,
        0,
        0);

    auto header = GetHeaders(request);
    auto content = request->Content->ReadAsString();
    auto sendRequest = HttpSendRequest(
        hrequest,
        header.c_str(),
        header.size(),
        (LPVOID)content.c_str(),
        content.size());

    auto response = std::make_shared<HttpResponseMessage>();
    if (sendRequest)
    {
        DWORD blocksize = 4096;
        DWORD received = 0;
        std::string block(blocksize, 0);
        std::string result;
        while (InternetReadFile(hrequest, &block[0], blocksize, &received) && received)
        {
            block.resize(received);
            result += block;
        }
        std::vector<std::byte> data(result.size() + 1);
        std::transform(
            result.begin(),
            result.end(),
            data.begin(),
            [](char c) { return std::byte(c); });

        response->Content = std::make_shared<ByteArrayContent>(data);
    }

    if (hrequest) InternetCloseHandle(hrequest);
    if (hconnect) InternetCloseHandle(hconnect);
    if (hsession) InternetCloseHandle(hsession);

    return response;
}
