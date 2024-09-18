#ifndef HTTPRESPONSEMESSAGE_HPP
#define HTTPRESPONSEMESSAGE_HPP

#include "httpcontent.hpp"
#include "httpstatuscode.hpp"
#include <map>
#include <memory>
#include <string>

class HttpResponseMessage
{
public:
    HttpResponseMessage();

    virtual ~HttpResponseMessage();

public:
    std::map<std::string, std::string> Headers;
    HttpStatusCode StatusCode;
    std::shared_ptr<HttpContent> Content;

public:
    bool IsSuccessStatusCode();
};

#endif // HTTPRESPONSEMESSAGE_HPP
