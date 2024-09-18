#ifndef BYTEARRAYCONTENT_HPP
#define BYTEARRAYCONTENT_HPP

#include "httpcontent.hpp"

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
    virtual std::shared_ptr<std::istream> OnCreateContentReadStream();

private:
    std::shared_ptr<struct imemstream> _stream;
};

#endif // BYTEARRAYCONTENT_HPP
