#include "httpmessagehandler.hpp"

#include "bytearraycontent.hpp"
#define WIN32_LEAN_AND_MEAN 1
#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <windows.h>
#include <wininet.h>

HttpMessageHandler::HttpMessageHandler() = default;

std::shared_ptr<HttpResponseMessage> HttpMessageHandler::Send(
    const std::shared_ptr<HttpRequestMessage> &request)
{
    static HINTERNET hInternet;
    hInternet = InternetOpenA("wininet-test", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

    const char *from = request->RequestUri.c_str();

    std::stringstream headers;

    for (auto &header : request->Headers())
    {
        headers << header.first << ": " << header.second << "\n";
    }

    auto headersString = headers.str();

    DWORD httpcode = 0;
    DWORD dwordlen = sizeof(DWORD);
    DWORD zero = 0;
    HINTERNET hUrl = InternetOpenUrlA(
        hInternet, from, headersString.c_str(), headersString.size(),
        INTERNET_FLAG_HYPERLINK |
            INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
            INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
            INTERNET_FLAG_NO_CACHE_WRITE |
            INTERNET_FLAG_NO_COOKIES |
            INTERNET_FLAG_NO_UI |
            INTERNET_FLAG_RESYNCHRONIZE |
            INTERNET_FLAG_RELOAD |
            INTERNET_FLAG_SECURE,
        0);

    auto response = std::make_shared<HttpResponseMessage>();

    if (!hUrl)
    {
        printf("InternetOpenUrl failed. err=%d", (int)GetLastError());
    }
    else if (!HttpQueryInfo(hUrl, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &httpcode, &dwordlen, &zero))
    {
        printf("HttpQueryInfo failed. err=%d", (int)GetLastError());
    }
    else if (httpcode != 200)
    {
        printf("HTTP request failed with response code %d", (int)httpcode);
    }
    else
    {
        std::stringstream ss;

        while (1)
        {
            DWORD br = 0;
            BYTE buf[1024 * 64];
            if (!InternetReadFile(hUrl, buf, sizeof(buf), &br))
            {
                printf("InternetReadFile failed. err=%d", (int)GetLastError());
                break;
            }
            else if (br == 0)
            {
                break; /* done! */
            }
            else
            {
                ss.write((char *)buf, br);
            }
        }

        auto s = ss.str();
        std::vector<std::byte> data(s.size() + 1);
        std::transform(
            s.begin(),
            s.end(),
            data.begin(),
            [](char c) { return std::byte(c); });

        response->Content = std::make_shared<ByteArrayContent>(data);
    }

    InternetCloseHandle(hUrl);

    return response;
}
