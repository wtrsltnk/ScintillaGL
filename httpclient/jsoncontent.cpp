#include "jsoncontent.hpp"

#include "imemstream.h"

JsonContent::JsonContent(
    const nlohmann::json &data)
{
    auto str = data.dump();
    std::vector<uint8_t> bytes(str.begin(), str.end());

    _stream = std::make_shared<imemstream>(reinterpret_cast<char const *>(bytes.data()), bytes.size());
}

JsonContent::~JsonContent() = default;

std::shared_ptr<std::istream> JsonContent::OnCreateContentReadStream()
{
    return _stream;
}
