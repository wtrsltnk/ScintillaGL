#ifndef HTTPCONTENT_HPP
#define HTTPCONTENT_HPP

#include <istream>
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
    std::shared_ptr<std::istream> CreateContentReadStream();

    std::vector<std::byte> ReadAsByteArray();

    std::string ReadAsString();

protected:
    virtual std::shared_ptr<std::istream> OnCreateContentReadStream() = 0;
};

#endif // HTTPCONTENT_HPP
