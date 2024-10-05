#ifndef HTTPCONTENT_HPP
#define HTTPCONTENT_HPP

#include <map>
#include <string>
#include <vector>

class HttpContent
{
public:
    HttpContent();

    virtual ~HttpContent();

public:
    std::map<std::string, std::string> Headers;

public:
    std::vector<std::byte> ReadAsByteArray();

    std::string ReadAsString();

protected:
    virtual const std::vector<std::byte> &Stream() = 0;
};

#endif // HTTPCONTENT_HPP
