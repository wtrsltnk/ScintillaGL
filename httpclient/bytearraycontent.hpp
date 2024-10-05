#ifndef BYTEARRAYCONTENT_HPP
#define BYTEARRAYCONTENT_HPP

#include "httpcontent.hpp"

#include <vector>

class ByteArrayContent : public HttpContent
{
public:
    ByteArrayContent(
        const std::vector<std::byte> &bytes);

    ByteArrayContent(
        const std::vector<std::byte> &bytes,
        size_t offset,
        size_t size);

    virtual ~ByteArrayContent();

protected:
    const std::vector<std::byte> &Stream();

protected:
    std::vector<std::byte> _stream;
};

#endif // BYTEARRAYCONTENT_HPP
