#include "bytearraycontent.hpp"

ByteArrayContent::ByteArrayContent(
    const std::vector<std::byte> &bytes)
    : ByteArrayContent(bytes, 0, bytes.size())
{}

ByteArrayContent::ByteArrayContent(
    const std::vector<std::byte> &bytes,
    size_t offset,
    size_t size)
{
    _stream.reserve(size);
    _stream.insert(_stream.begin(), bytes.begin() + offset, bytes.begin() + offset + size);
}

ByteArrayContent::~ByteArrayContent() = default;

const std::vector<std::byte> &ByteArrayContent::Stream()
{
    return _stream;
}
