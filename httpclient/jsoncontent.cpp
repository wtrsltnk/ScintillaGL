#include "jsoncontent.hpp"

JsonContent::JsonContent(
    const nlohmann::json &data)
{
    auto str = data.dump();

    _stream.resize(str.size());
    std::transform(str.begin(), str.end(), _stream.begin(),
                   [](char c) { return std::byte(c); });
}

JsonContent::~JsonContent() = default;

const std::vector<std::byte> &JsonContent::Stream()
{
    return _stream;
}
