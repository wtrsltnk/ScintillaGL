#include "bytearraycontent.hpp"

#include "imemstream.h"

ByteArrayContent::ByteArrayContent(
    const std::vector<std::byte> &bytes)
    : ByteArrayContent(bytes, 0, bytes.size())
{}

ByteArrayContent::ByteArrayContent(
    const std::vector<std::byte> &bytes,
    size_t offset,
    size_t size)
{
    _stream = std::make_shared<imemstream>(reinterpret_cast<char const *>(bytes.data() + offset), bytes.size());
}

ByteArrayContent::~ByteArrayContent() = default;

std::shared_ptr<std::istream> ByteArrayContent::OnCreateContentReadStream()
{
    return _stream;
}
