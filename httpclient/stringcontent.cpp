#include "stringcontent.hpp"

#include <algorithm>

std::vector<std::byte> convert(
    const std::string &content)
{
    std::vector<std::byte> data(content.size() + 1);

    std::transform(
        content.begin(),
        content.end(),
        data.begin(),
        [](char c) { return std::byte(c); });

    return data;
}

StringContent::StringContent(
    const std::string &content)
    : ByteArrayContent(convert(content))
{}

StringContent::~StringContent() = default;
