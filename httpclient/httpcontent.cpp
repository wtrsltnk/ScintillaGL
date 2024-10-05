#include "httpcontent.hpp"

HttpContent::HttpContent() {}

HttpContent::~HttpContent() {}

std::vector<std::byte> HttpContent::ReadAsByteArray()
{
    return Stream();
}

std::string HttpContent::ReadAsString()
{
    auto &s = Stream();

    if (s.empty())
    {
        return "";
    }

    return std::string(reinterpret_cast<const char *>(&s[0]), s.size());
}
